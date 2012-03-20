# -*- coding: utf-8 -*-
#--
# Copyright (C) 2004 Mauricio Julio Fernández Pradier
# See LICENSE.txt for additional licensing information.
#++

##
#--
# struct tarfile_entry_posix {
#   char name[100];     # ASCII + (Z unless filled)
#   char mode[8];       # 0 padded, octal, null
#   char uid[8];        # ditto
#   char gid[8];        # ditto
#   char size[12];      # 0 padded, octal, null
#   char mtime[12];     # 0 padded, octal, null
#   char checksum[8];   # 0 padded, octal, null, space
#   char typeflag[1];   # file: "0"  dir: "5"
#   char linkname[100]; # ASCII + (Z unless filled)
#   char magic[6];      # "ustar\0"
#   char version[2];    # "00"
#   char uname[32];     # ASCIIZ
#   char gname[32];     # ASCIIZ
#   char devmajor[8];   # 0 padded, octal, null
#   char devminor[8];   # o padded, octal, null
#   char prefix[155];   # ASCII + (Z unless filled)
# };
#++
# A header for a tar file

class Gem::Package::TarHeader

  ##
  # Fields in the tar header

  FIELDS = [
    :checksum,
    :devmajor,
    :devminor,
    :gid,
    :gname,
    :linkname,
    :magic,
    :mode,
    :mtime,
    :name,
    :prefix,
    :size,
    :typeflag,
    :uid,
    :uname,
    :version,
  ]

  ##
  # Pack format for a tar header

  PACK_FORMAT = 'a100' + # name
                'a8'   + # mode
                'a8'   + # uid
                'a8'   + # gid
                'a12'  + # size
                'a12'  + # mtime
                'a7a'  + # chksum
                'a'    + # typeflag
                'a100' + # linkname
                'a6'   + # magic
                'a2'   + # version
                'a32'  + # uname
                'a32'  + # gname
                'a8'   + # devmajor
                'a8'   + # devminor
                'a155'   # prefix

  ##
  # Unpack format for a tar header

  UNPACK_FORMAT = 'A100' + # name
                  'A8'   + # mode
                  'A8'   + # uid
                  'A8'   + # gid
                  'A12'  + # size
                  'A12'  + # mtime
                  'A8'   + # checksum
                  'A'    + # typeflag
                  'A100' + # linkname
                  'A6'   + # magic
                  'A2'   + # version
                  'A32'  + # uname
                  'A32'  + # gname
                  'A8'   + # devmajor
                  'A8'   + # devminor
                  'A155'   # prefix

  attr_reader(*FIELDS)

  ##
  # Creates a tar header from IO +stream+

  def self.from(stream)
    header = stream.read 512
    empty = (header == "\0" * 512)

    fields = header.unpack UNPACK_FORMAT

    name     = fields.shift
    mode     = fields.shift.oct
    uid      = fields.shift.oct
    gid      = fields.shift.oct
    size     = fields.shift.oct
    mtime    = fields.shift.oct
    checksum = fields.shift.oct
    typeflag = fields.shift
    linkname = fields.shift
    magic    = fields.shift
    version  = fields.shift.oct
    uname    = fields.shift
    gname    = fields.shift
    devmajor = fields.shift.oct
    devminor = fields.shift.oct
    prefix   = fields.shift

    new :name     => name,
        :mode     => mode,
        :uid      => uid,
        :gid      => gid,
        :size     => size,
        :mtime    => mtime,
        :checksum => checksum,
        :typeflag => typeflag,
        :linkname => linkname,
        :magic    => magic,
        :version  => version,
        :uname    => uname,
        :gname    => gname,
        :devmajor => devmajor,
        :devminor => devminor,
        :prefix   => prefix,

        :empty    => empty

    # HACK unfactor for Rubinius
    #new :name     => fields.shift,
    #    :mode     => fields.shift.oct,
    #    :uid      => fields.shift.oct,
    #    :gid      => fields.shift.oct,
    #    :size     => fields.shift.oct,
    #    :mtime    => fields.shift.oct,
    #    :checksum => fields.shift.oct,
    #    :typeflag => fields.shift,
    #    :linkname => fields.shift,
    #    :magic    => fields.shift,
    #    :version  => fields.shift.oct,
    #    :uname    => fields.shift,
    #    :gname    => fields.shift,
    #    :devmajor => fields.shift.oct,
    #    :devminor => fields.shift.oct,
    #    :prefix   => fields.shift,

    #    :empty => empty
  end

  ##
  # Creates a new TarHeader using +vals+

  def initialize(vals)
    unless vals[:name] && vals[:size] && vals[:prefix] && vals[:mode] then
      raise ArgumentError, ":name, :size, :prefix and :mode required"
    end

    vals[:uid] ||= 0
    vals[:gid] ||= 0
    vals[:mtime] ||= 0
    vals[:checksum] ||= ""
    vals[:typeflag] ||= "0"
    vals[:magic] ||= "ustar"
    vals[:version] ||= "00"
    vals[:uname] ||= "wheel"
    vals[:gname] ||= "wheel"
    vals[:devmajor] ||= 0
    vals[:devminor] ||= 0

    FIELDS.each do |name|
      instance_variable_set "@#{name}", vals[name]
    end

    @empty = vals[:empty]
  end

  ##
  # Is the tar entry empty?

  def empty?
    @empty
  end

  def ==(other) # :nodoc:
    self.class === other and
    @checksum == other.checksum and
    @devmajor == other.devmajor and
    @devminor == other.devminor and
    @gid      == other.gid      and
    @gname    == other.gname    and
    @linkname == other.linkname and
    @magic    == other.magic    and
    @mode     == other.mode     and
    @mtime    == other.mtime    and
    @name     == other.name     and
    @prefix   == other.prefix   and
    @size     == other.size     and
    @typeflag == other.typeflag and
    @uid      == other.uid      and
    @uname    == other.uname    and
    @version  == other.version
  end

  def to_s # :nodoc:
    update_checksum
    header
  end

  ##
  # Updates the TarHeader's checksum

  def update_checksum
    header = header " " * 8
    @checksum = oct calculate_checksum(header), 6
  end

  private

  def calculate_checksum(header)
    header.unpack("C*").inject { |a, b| a + b }
  end

  def header(checksum = @checksum)
    header = [
      name,
      oct(mode, 7),
      oct(uid, 7),
      oct(gid, 7),
      oct(size, 11),
      oct(mtime, 11),
      checksum,
      " ",
      typeflag,
      linkname,
      magic,
      oct(version, 2),
      uname,
      gname,
      oct(devmajor, 7),
      oct(devminor, 7),
      prefix
    ]

    header = header.pack PACK_FORMAT

    header << ("\0" * ((512 - header.size) % 512))
  end

  def oct(num, len)
    "%0#{len}o" % num
  end

end

