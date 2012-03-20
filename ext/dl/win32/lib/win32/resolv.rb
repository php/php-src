=begin
= Win32 DNS and DHCP I/F

=end

require "dl/import"
require 'win32/registry'

module Win32
  module Resolv
    API = Registry::API

    def self.get_hosts_path
      path = get_hosts_dir
      path = File.expand_path('hosts', path)
      File.exist?(path) ? path : nil
    end

    def self.get_resolv_info
      search, nameserver = get_info
      if search.empty?
        search = nil
      else
        search.delete("")
        search.uniq!
      end
      if nameserver.empty?
        nameserver = nil
      else
        nameserver.delete("")
        nameserver.delete("0.0.0.0")
        nameserver.uniq!
      end
      [ search, nameserver ]
    end

module Kernel32
  extend DL::Importer
  dlload "kernel32"
end
getv = Kernel32.extern "int GetVersionExA(void *)", :stdcall
info = [ 148, 0, 0, 0, 0 ].pack('V5') + "\0" * 128
getv.call(info)
if info.unpack('V5')[4] == 2  # VER_PLATFORM_WIN32_NT
#====================================================================
# Windows NT
#====================================================================
  module_eval <<-'__EOS__', __FILE__, __LINE__+1
    TCPIP_NT = 'SYSTEM\CurrentControlSet\Services\Tcpip\Parameters'

    class << self
      private
      def get_hosts_dir
        Registry::HKEY_LOCAL_MACHINE.open(TCPIP_NT) do |reg|
          reg.read_s_expand('DataBasePath')
        end
      end

      def get_info
        search = nil
        nameserver = []
        Registry::HKEY_LOCAL_MACHINE.open(TCPIP_NT) do |reg|
          begin
            slist = reg.read_s('SearchList')
            search = slist.split(/,\s*/) unless slist.empty?
          rescue Registry::Error
          end

          if add_search = search.nil?
            search = []
            begin
              nvdom = reg.read_s('NV Domain')
              unless nvdom.empty?
                @search = [ nvdom ]
                if reg.read_i('UseDomainNameDevolution') != 0
                  if /^\w+\./ =~ nvdom
                    devo = $'
                  end
                end
              end
            rescue Registry::Error
            end
          end

          reg.open('Interfaces') do |h|
            h.each_key do |iface,|
              h.open(iface) do |regif|
                begin
                  [ 'NameServer', 'DhcpNameServer' ].each do |key|
                    begin
                      ns = regif.read_s(key)
                    rescue
                    else
                      unless ns.empty?
                        nameserver.concat(ns.split(/[,\s]\s*/))
                        break
                      end
                    end
                  end
                rescue Registry::Error
                end

                if add_search
                  begin
                    [ 'Domain', 'DhcpDomain' ].each do |key|
                      dom = regif.read_s(key)
                      unless dom.empty?
                        search.concat(dom.split(/,\s*/))
                        break
                      end
                    end
                  rescue Registry::Error
                  end
                end
              end
            end
          end
          search << devo if add_search and devo
        end
        [ search.uniq, nameserver.uniq ]
      end
    end
  __EOS__
else
#====================================================================
# Windows 9x
#====================================================================
  module_eval <<-'__EOS__', __FILE__, __LINE__+1
    TCPIP_9X = 'SYSTEM\CurrentControlSet\Services\VxD\MSTCP'
    DHCP_9X = 'SYSTEM\CurrentControlSet\Services\VxD\DHCP'
    WINDOWS = 'Software\Microsoft\Windows\CurrentVersion'

    class << self
   #   private

      def get_hosts_dir
        Registry::HKEY_LOCAL_MACHINE.open(WINDOWS) do |reg|
          reg.read_s_expand('SystemRoot')
        end
      end

      def get_info
        search = []
        nameserver = []
        begin
          Registry::HKEY_LOCAL_MACHINE.open(TCPIP_9X) do |reg|
            if reg.read_s("EnableDNS") == "1"
              domain = reg.read_s("Domain")
              ns = reg.read_s("NameServer")
              slist = reg.read_s("SearchList")
              search << domain unless domain.empty?
              search.concat(slist.split(/,\s*/))
              nameserver.concat(ns.split(/[,\s]\s*/))
            end
          end
        rescue Registry::Error
        end

        dhcpinfo = get_dhcpinfo
        search.concat(dhcpinfo[0])
        nameserver.concat(dhcpinfo[1])
        [ search, nameserver ]
      end

      def get_dhcpinfo
        macaddrs = {}
        ipaddrs = {}
        WsControl.get_iflist.each do |index, macaddr, *ipaddr|
          macaddrs[macaddr] = 1
          ipaddr.each { |ipaddr| ipaddrs[ipaddr] = 1 }
        end
        iflist = [ macaddrs, ipaddrs ]

        search = []
        nameserver = []
        version = -1
        Registry::HKEY_LOCAL_MACHINE.open(DHCP_9X) do |reg|
          begin
            version = API.unpackdw(reg.read_bin("Version"))
          rescue Registry::Error
          end

          reg.each_key do |key,|
            catch(:not_used) do
              reg.open(key) do |regdi|
                dom, ns = get_dhcpinfo_key(version, regdi, iflist)
                search << dom if dom
                nameserver.concat(ns) if ns
              end
            end
          end
        end
        [ search, nameserver ]
      end

      def get_dhcpinfo_95(reg)
        dhcp = reg.read_bin("DhcpInfo")
        [
          API.unpackdw(dhcp[4..7]),
          API.unpackdw(dhcp[8..11]),
          1,
          dhcp[45..50],
          reg.read_bin("OptionInfo"),
        ]
      end

      def get_dhcpinfo_98(reg)
        [
          API.unpackdw(reg.read_bin("DhcpIPAddress")),
          API.unpackdw(reg.read_bin("DhcpSubnetMask")),
          API.unpackdw(reg.read_bin("HardwareType")),
          reg.read_bin("HardwareAddress"),
          reg.read_bin("OptionInfo"),
        ]
      end

      def get_dhcpinfo_key(version, reg, iflist)
        info = case version
               when 1
                 get_dhcpinfo_95(reg)
               when 2
                 get_dhcpinfo_98(reg)
               else
                 begin
                   get_dhcpinfo_98(reg)
                 rescue Registry::Error
                   get_dhcpinfo_95(reg)
                 end
               end
        ipaddr, netmask, hwtype, macaddr, opt = info
        throw :not_used unless
          ipaddr and ipaddr != 0 and
          netmask and netmask != 0 and
          macaddr and macaddr.size == 6 and
          hwtype == 1 and
          iflist[0][macaddr] and iflist[1][ipaddr]

        size = opt.size
        idx = 0
        while idx <= size
          opttype = opt[idx]
          optsize = opt[idx + 1]
          optval  = opt[idx + 2, optsize]
          case opttype
          when 0xFF    ## term
            break
          when 0x0F    ## domain
            domain = optval.chomp("\0")
          when 0x06    ## dns
            nameserver = optval.scan(/..../).collect { |addr|
              "%d.%d.%d.%d" % addr.unpack('C4')
            }
          end
          idx += optsize + 2
        end
        [ domain, nameserver ]
      rescue Registry::Error
        throw :not_used
      end
    end

    module WsControl
      module WSock32
        extend DL::Importer
        dlload "wsock32.dll"
      end
      WsControl = WSock32.extern "int WsControl(int, int, void *, void *, void *, void *", :stdcall
      WSAGetLastError = WSock32.extern "int WSAGetLastError(void)", :stdcall

      MAX_TDI_ENTITIES = 512
      IPPROTO_TCP = 6
      WSCTL_TCP_QUERY_INFORMATION = 0
      INFO_CLASS_GENERIC = 0x100
      INFO_CLASS_PROTOCOL = 0x200
      INFO_TYPE_PROVIDER = 0x100
      ENTITY_LIST_ID = 0
      GENERIC_ENTITY = 0
      CL_NL_ENTITY = 0x301
      IF_ENTITY = 0x200
      ENTITY_TYPE_ID = 1
      CL_NL_IP = 0x303
      IF_MIB = 0x202
      IF_MIB_STATS_ID = 1
      IP_MIB_ADDRTABLE_ENTRY_ID = 0x102

      def self.wsctl(tei_entity, tei_instance,
                     toi_class, toi_type, toi_id,
                     buffsize)
        reqinfo = [
                  ## TDIEntityID
                    tei_entity, tei_instance,
                  ## TDIObjectID
                    toi_class, toi_type, toi_id,
                  ## TCP_REQUEST_INFORMATION_EX
                    ""
                  ].pack('VVVVVa16')
        reqsize = API.packdw(reqinfo.size)
        buff = "\0" * buffsize
        buffsize = API.packdw(buffsize)
        result = WsControl.call(
                   IPPROTO_TCP,
                   WSCTL_TCP_QUERY_INFORMATION,
                   reqinfo, reqsize,
                   buff, buffsize)
        if result != 0
          raise RuntimeError, "WsControl failed.(#{result})"
        end
        [ buff, API.unpackdw(buffsize) ]
      end
      private_class_method :wsctl

      def self.get_iflist
        # Get TDI Entity List
        entities, size =
          wsctl(GENERIC_ENTITY, 0,
                INFO_CLASS_GENERIC,
                INFO_TYPE_PROVIDER,
                ENTITY_LIST_ID,
                MAX_TDI_ENTITIES * 8)  # sizeof(TDIEntityID)
        entities = entities[0, size].
                     scan(/.{8}/).
                     collect { |e| e.unpack('VV') }

        # Get MIB Interface List
        iflist = []
        ifcount = 0
        entities.each do |entity, instance|
          if( (entity & IF_ENTITY)>0 )
            ifcount += 1
            etype, = wsctl(entity, instance,
                           INFO_CLASS_GENERIC,
                           INFO_TYPE_PROVIDER,
                           ENTITY_TYPE_ID,
                           4)
            if( (API.unpackdw(etype) & IF_MIB)==IF_MIB )
              ifentry, = wsctl(entity, instance,
                               INFO_CLASS_PROTOCOL,
                               INFO_TYPE_PROVIDER,
                               IF_MIB_STATS_ID,
                               21 * 4 + 8 + 130)  # sizeof(IFEntry)
              iflist << [
                API.unpackdw(ifentry[0,4]),
                ifentry[20, 6]
              ]
            end
          end
        end

        # Get IP Addresses
        entities.each do |entity, instance|
          if entity == CL_NL_ENTITY
            etype, = wsctl(entity, instance,
                           INFO_CLASS_GENERIC,
                           INFO_TYPE_PROVIDER,
                           ENTITY_TYPE_ID,
                           4)
            if API.unpackdw(etype) == CL_NL_IP
              ipentries, = wsctl(entity, instance,
                                 INFO_CLASS_PROTOCOL,
                                 INFO_TYPE_PROVIDER,
                                 IP_MIB_ADDRTABLE_ENTRY_ID,
                                 24 * (ifcount+1))  # sizeof(IPAddrEntry)
              ipentries.scan(/.{24}/) do |ipentry|
                ipaddr, index = ipentry.unpack('VV')
                if ifitem = iflist.assoc(index)
                  ifitem << ipaddr
                end
              end
            end
          end
        end
        iflist
      end
    end
  __EOS__
end
#====================================================================
  end
end
