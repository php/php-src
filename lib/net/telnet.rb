# = net/telnet.rb - Simple Telnet Client Library
#
# Author:: Wakou Aoyama <wakou@ruby-lang.org>
# Documentation:: William Webber and Wakou Aoyama
#
# This file holds the class Net::Telnet, which provides client-side
# telnet functionality.
#
# For documentation, see Net::Telnet.
#

require "net/protocol"
require "English"

module Net

  #
  # == Net::Telnet
  #
  # Provides telnet client functionality.
  #
  # This class also has, through delegation, all the methods of a
  # socket object (by default, a +TCPSocket+, but can be set by the
  # +Proxy+ option to <tt>new()</tt>).  This provides methods such as
  # <tt>close()</tt> to end the session and <tt>sysread()</tt> to read
  # data directly from the host, instead of via the <tt>waitfor()</tt>
  # mechanism.  Note that if you do use <tt>sysread()</tt> directly
  # when in telnet mode, you should probably pass the output through
  # <tt>preprocess()</tt> to extract telnet command sequences.
  #
  # == Overview
  #
  # The telnet protocol allows a client to login remotely to a user
  # account on a server and execute commands via a shell.  The equivalent
  # is done by creating a Net::Telnet class with the +Host+ option
  # set to your host, calling #login() with your user and password,
  # issuing one or more #cmd() calls, and then calling #close()
  # to end the session.  The #waitfor(), #print(), #puts(), and
  # #write() methods, which #cmd() is implemented on top of, are
  # only needed if you are doing something more complicated.
  #
  # A Net::Telnet object can also be used to connect to non-telnet
  # services, such as SMTP or HTTP.  In this case, you normally
  # want to provide the +Port+ option to specify the port to
  # connect to, and set the +Telnetmode+ option to false to prevent
  # the client from attempting to interpret telnet command sequences.
  # Generally, #login() will not work with other protocols, and you
  # have to handle authentication yourself.
  #
  # For some protocols, it will be possible to specify the +Prompt+
  # option once when you create the Telnet object and use #cmd() calls;
  # for others, you will have to specify the response sequence to
  # look for as the Match option to every #cmd() call, or call
  # #puts() and #waitfor() directly; for yet others, you will have
  # to use #sysread() instead of #waitfor() and parse server
  # responses yourself.
  #
  # It is worth noting that when you create a new Net::Telnet object,
  # you can supply a proxy IO channel via the Proxy option.  This
  # can be used to attach the Telnet object to other Telnet objects,
  # to already open sockets, or to any read-write IO object.  This
  # can be useful, for instance, for setting up a test fixture for
  # unit testing.
  #
  # == Examples
  #
  # === Log in and send a command, echoing all output to stdout
  #
  #   localhost = Net::Telnet::new("Host" => "localhost",
  #                                "Timeout" => 10,
  #                                "Prompt" => /[$%#>] \z/n)
  #   localhost.login("username", "password") { |c| print c }
  #   localhost.cmd("command") { |c| print c }
  #   localhost.close
  #
  #
  # === Check a POP server to see if you have mail
  #
  #   pop = Net::Telnet::new("Host" => "your_destination_host_here",
  #                          "Port" => 110,
  #                          "Telnetmode" => false,
  #                          "Prompt" => /^\+OK/n)
  #   pop.cmd("user " + "your_username_here") { |c| print c }
  #   pop.cmd("pass " + "your_password_here") { |c| print c }
  #   pop.cmd("list") { |c| print c }
  #
  # == References
  #
  # There are a large number of RFCs relevant to the Telnet protocol.
  # RFCs 854-861 define the base protocol.  For a complete listing
  # of relevant RFCs, see
  # http://www.omnifarious.org/~hopper/technical/telnet-rfc.html
  #
  class Telnet

    # :stopdoc:
    IAC   = 255.chr # "\377" # "\xff" # interpret as command
    DONT  = 254.chr # "\376" # "\xfe" # you are not to use option
    DO    = 253.chr # "\375" # "\xfd" # please, you use option
    WONT  = 252.chr # "\374" # "\xfc" # I won't use option
    WILL  = 251.chr # "\373" # "\xfb" # I will use option
    SB    = 250.chr # "\372" # "\xfa" # interpret as subnegotiation
    GA    = 249.chr # "\371" # "\xf9" # you may reverse the line
    EL    = 248.chr # "\370" # "\xf8" # erase the current line
    EC    = 247.chr # "\367" # "\xf7" # erase the current character
    AYT   = 246.chr # "\366" # "\xf6" # are you there
    AO    = 245.chr # "\365" # "\xf5" # abort output--but let prog finish
    IP    = 244.chr # "\364" # "\xf4" # interrupt process--permanently
    BREAK = 243.chr # "\363" # "\xf3" # break
    DM    = 242.chr # "\362" # "\xf2" # data mark--for connect. cleaning
    NOP   = 241.chr # "\361" # "\xf1" # nop
    SE    = 240.chr # "\360" # "\xf0" # end sub negotiation
    EOR   = 239.chr # "\357" # "\xef" # end of record (transparent mode)
    ABORT = 238.chr # "\356" # "\xee" # Abort process
    SUSP  = 237.chr # "\355" # "\xed" # Suspend process
    EOF   = 236.chr # "\354" # "\xec" # End of file
    SYNCH = 242.chr # "\362" # "\xf2" # for telfunc calls

    OPT_BINARY         =   0.chr # "\000" # "\x00" # Binary Transmission
    OPT_ECHO           =   1.chr # "\001" # "\x01" # Echo
    OPT_RCP            =   2.chr # "\002" # "\x02" # Reconnection
    OPT_SGA            =   3.chr # "\003" # "\x03" # Suppress Go Ahead
    OPT_NAMS           =   4.chr # "\004" # "\x04" # Approx Message Size Negotiation
    OPT_STATUS         =   5.chr # "\005" # "\x05" # Status
    OPT_TM             =   6.chr # "\006" # "\x06" # Timing Mark
    OPT_RCTE           =   7.chr # "\a"   # "\x07" # Remote Controlled Trans and Echo
    OPT_NAOL           =   8.chr # "\010" # "\x08" # Output Line Width
    OPT_NAOP           =   9.chr # "\t"   # "\x09" # Output Page Size
    OPT_NAOCRD         =  10.chr # "\n"   # "\x0a" # Output Carriage-Return Disposition
    OPT_NAOHTS         =  11.chr # "\v"   # "\x0b" # Output Horizontal Tab Stops
    OPT_NAOHTD         =  12.chr # "\f"   # "\x0c" # Output Horizontal Tab Disposition
    OPT_NAOFFD         =  13.chr # "\r"   # "\x0d" # Output Formfeed Disposition
    OPT_NAOVTS         =  14.chr # "\016" # "\x0e" # Output Vertical Tabstops
    OPT_NAOVTD         =  15.chr # "\017" # "\x0f" # Output Vertical Tab Disposition
    OPT_NAOLFD         =  16.chr # "\020" # "\x10" # Output Linefeed Disposition
    OPT_XASCII         =  17.chr # "\021" # "\x11" # Extended ASCII
    OPT_LOGOUT         =  18.chr # "\022" # "\x12" # Logout
    OPT_BM             =  19.chr # "\023" # "\x13" # Byte Macro
    OPT_DET            =  20.chr # "\024" # "\x14" # Data Entry Terminal
    OPT_SUPDUP         =  21.chr # "\025" # "\x15" # SUPDUP
    OPT_SUPDUPOUTPUT   =  22.chr # "\026" # "\x16" # SUPDUP Output
    OPT_SNDLOC         =  23.chr # "\027" # "\x17" # Send Location
    OPT_TTYPE          =  24.chr # "\030" # "\x18" # Terminal Type
    OPT_EOR            =  25.chr # "\031" # "\x19" # End of Record
    OPT_TUID           =  26.chr # "\032" # "\x1a" # TACACS User Identification
    OPT_OUTMRK         =  27.chr # "\e"   # "\x1b" # Output Marking
    OPT_TTYLOC         =  28.chr # "\034" # "\x1c" # Terminal Location Number
    OPT_3270REGIME     =  29.chr # "\035" # "\x1d" # Telnet 3270 Regime
    OPT_X3PAD          =  30.chr # "\036" # "\x1e" # X.3 PAD
    OPT_NAWS           =  31.chr # "\037" # "\x1f" # Negotiate About Window Size
    OPT_TSPEED         =  32.chr # " "    # "\x20" # Terminal Speed
    OPT_LFLOW          =  33.chr # "!"    # "\x21" # Remote Flow Control
    OPT_LINEMODE       =  34.chr # "\""   # "\x22" # Linemode
    OPT_XDISPLOC       =  35.chr # "#"    # "\x23" # X Display Location
    OPT_OLD_ENVIRON    =  36.chr # "$"    # "\x24" # Environment Option
    OPT_AUTHENTICATION =  37.chr # "%"    # "\x25" # Authentication Option
    OPT_ENCRYPT        =  38.chr # "&"    # "\x26" # Encryption Option
    OPT_NEW_ENVIRON    =  39.chr # "'"    # "\x27" # New Environment Option
    OPT_EXOPL          = 255.chr # "\377" # "\xff" # Extended-Options-List

    NULL = "\000"
    CR   = "\015"
    LF   = "\012"
    EOL  = CR + LF
    REVISION = '$Id$'
    # :startdoc:

    #
    # Creates a new Net::Telnet object.
    #
    # Attempts to connect to the host (unless the Proxy option is
    # provided: see below).  If a block is provided, it is yielded
    # status messages on the attempt to connect to the server, of
    # the form:
    #
    #   Trying localhost...
    #   Connected to localhost.
    #
    # +options+ is a hash of options.  The following example lists
    # all options and their default values.
    #
    #   host = Net::Telnet::new(
    #            "Host"       => "localhost",  # default: "localhost"
    #            "Port"       => 23,           # default: 23
    #            "Binmode"    => false,        # default: false
    #            "Output_log" => "output_log", # default: nil (no output)
    #            "Dump_log"   => "dump_log",   # default: nil (no output)
    #            "Prompt"     => /[$%#>] \z/n, # default: /[$%#>] \z/n
    #            "Telnetmode" => true,         # default: true
    #            "Timeout"    => 10,           # default: 10
    #              # if ignore timeout then set "Timeout" to false.
    #            "Waittime"   => 0,            # default: 0
    #            "Proxy"      => proxy         # default: nil
    #                            # proxy is Net::Telnet or IO object
    #          )
    #
    # The options have the following meanings:
    #
    # Host:: the hostname or IP address of the host to connect to, as a String.
    #        Defaults to "localhost".
    #
    # Port:: the port to connect to.  Defaults to 23.
    #
    # Binmode:: if false (the default), newline substitution is performed.
    #           Outgoing LF is
    #           converted to CRLF, and incoming CRLF is converted to LF.  If
    #           true, this substitution is not performed.  This value can
    #           also be set with the #binmode() method.  The
    #           outgoing conversion only applies to the #puts() and #print()
    #           methods, not the #write() method.  The precise nature of
    #           the newline conversion is also affected by the telnet options
    #           SGA and BIN.
    #
    # Output_log:: the name of the file to write connection status messages
    #              and all received traffic to.  In the case of a proper
    #              Telnet session, this will include the client input as
    #              echoed by the host; otherwise, it only includes server
    #              responses.  Output is appended verbatim to this file.
    #              By default, no output log is kept.
    #
    # Dump_log:: as for Output_log, except that output is written in hexdump
    #            format (16 bytes per line as hex pairs, followed by their
    #            printable equivalent), with connection status messages
    #            preceded by '#', sent traffic preceded by '>', and
    #            received traffic preceded by '<'.  By default, not dump log
    #            is kept.
    #
    # Prompt:: a regular expression matching the host's command-line prompt
    #          sequence.  This is needed by the Telnet class to determine
    #          when the output from a command has finished and the host is
    #          ready to receive a new command.  By default, this regular
    #          expression is /[$%#>] \z/n.
    #
    # Telnetmode:: a boolean value, true by default.  In telnet mode,
    #              traffic received from the host is parsed for special
    #              command sequences, and these sequences are escaped
    #              in outgoing traffic sent using #puts() or #print()
    #              (but not #write()).  If you are using the Net::Telnet
    #              object to connect to a non-telnet service (such as
    #              SMTP or POP), this should be set to "false" to prevent
    #              undesired data corruption.  This value can also be set
    #              by the #telnetmode() method.
    #
    # Timeout:: the number of seconds to wait before timing out both the
    #           initial attempt to connect to host (in this constructor),
    #           and all attempts to read data from the host (in #waitfor(),
    #           #cmd(), and #login()).  Exceeding this timeout causes a
    #           TimeoutError to be raised.  The default value is 10 seconds.
    #           You can disable the timeout by setting this value to false.
    #           In this case, the connect attempt will eventually timeout
    #           on the underlying connect(2) socket call with an
    #           Errno::ETIMEDOUT error (but generally only after a few
    #           minutes), but other attempts to read data from the host
    #           will hand indefinitely if no data is forthcoming.
    #
    # Waittime:: the amount of time to wait after seeing what looks like a
    #            prompt (that is, received data that matches the Prompt
    #            option regular expression) to see if more data arrives.
    #            If more data does arrive in this time, Net::Telnet assumes
    #            that what it saw was not really a prompt.  This is to try to
    #            avoid false matches, but it can also lead to missing real
    #            prompts (if, for instance, a background process writes to
    #            the terminal soon after the prompt is displayed).  By
    #            default, set to 0, meaning not to wait for more data.
    #
    # Proxy:: a proxy object to used instead of opening a direct connection
    #         to the host.  Must be either another Net::Telnet object or
    #         an IO object.  If it is another Net::Telnet object, this
    #         instance will use that one's socket for communication.  If an
    #         IO object, it is used directly for communication.  Any other
    #         kind of object will cause an error to be raised.
    #
    def initialize(options) # :yield: mesg
      @options = options
      @options["Host"]       = "localhost"   unless @options.has_key?("Host")
      @options["Port"]       = 23            unless @options.has_key?("Port")
      @options["Prompt"]     = /[$%#>] \z/n  unless @options.has_key?("Prompt")
      @options["Timeout"]    = 10            unless @options.has_key?("Timeout")
      @options["Waittime"]   = 0             unless @options.has_key?("Waittime")
      unless @options.has_key?("Binmode")
        @options["Binmode"]    = false
      else
        unless (true == @options["Binmode"] or false == @options["Binmode"])
          raise ArgumentError, "Binmode option must be true or false"
        end
      end

      unless @options.has_key?("Telnetmode")
        @options["Telnetmode"] = true
      else
        unless (true == @options["Telnetmode"] or false == @options["Telnetmode"])
          raise ArgumentError, "Telnetmode option must be true or false"
        end
      end

      @telnet_option = { "SGA" => false, "BINARY" => false }

      if @options.has_key?("Output_log")
        @log = File.open(@options["Output_log"], 'a+')
        @log.sync = true
        @log.binmode
      end

      if @options.has_key?("Dump_log")
        @dumplog = File.open(@options["Dump_log"], 'a+')
        @dumplog.sync = true
        @dumplog.binmode
        def @dumplog.log_dump(dir, x)  # :nodoc:
          len = x.length
          addr = 0
          offset = 0
          while 0 < len
            if len < 16
              line = x[offset, len]
            else
              line = x[offset, 16]
            end
            hexvals = line.unpack('H*')[0]
            hexvals += ' ' * (32 - hexvals.length)
            hexvals = format("%s %s %s %s  " * 4, *hexvals.unpack('a2' * 16))
            line = line.gsub(/[\000-\037\177-\377]/n, '.')
            printf "%s 0x%5.5x: %s%s\n", dir, addr, hexvals, line
            addr += 16
            offset += 16
            len -= 16
          end
          print "\n"
        end
      end

      if @options.has_key?("Proxy")
        if @options["Proxy"].kind_of?(Net::Telnet)
          @sock = @options["Proxy"].sock
        elsif @options["Proxy"].kind_of?(IO)
          @sock = @options["Proxy"]
        else
          raise "Error: Proxy must be an instance of Net::Telnet or IO."
        end
      else
        message = "Trying " + @options["Host"] + "...\n"
        yield(message) if block_given?
        @log.write(message) if @options.has_key?("Output_log")
        @dumplog.log_dump('#', message) if @options.has_key?("Dump_log")

        begin
          if @options["Timeout"] == false
            @sock = TCPSocket.open(@options["Host"], @options["Port"])
          else
            Timeout.timeout(@options["Timeout"], Net::OpenTimeout) do
              @sock = TCPSocket.open(@options["Host"], @options["Port"])
            end
          end
        rescue Net::OpenTimeout
          raise Net::OpenTimeout, "timed out while opening a connection to the host"
        rescue
          @log.write($ERROR_INFO.to_s + "\n") if @options.has_key?("Output_log")
          @dumplog.log_dump('#', $ERROR_INFO.to_s + "\n") if @options.has_key?("Dump_log")
          raise
        end
        @sock.sync = true
        @sock.binmode

        message = "Connected to " + @options["Host"] + ".\n"
        yield(message) if block_given?
        @log.write(message) if @options.has_key?("Output_log")
        @dumplog.log_dump('#', message) if @options.has_key?("Dump_log")
      end

    end # initialize

    # The socket the Telnet object is using.  Note that this object becomes
    # a delegate of the Telnet object, so normally you invoke its methods
    # directly on the Telnet object.
    attr :sock

    # Set telnet command interpretation on (+mode+ == true) or off
    # (+mode+ == false), or return the current value (+mode+ not
    # provided).  It should be on for true telnet sessions, off if
    # using Net::Telnet to connect to a non-telnet service such
    # as SMTP.
    def telnetmode(mode = nil)
      case mode
      when nil
        @options["Telnetmode"]
      when true, false
        @options["Telnetmode"] = mode
      else
        raise ArgumentError, "argument must be true or false, or missing"
      end
    end

    # Turn telnet command interpretation on (true) or off (false).  It
    # should be on for true telnet sessions, off if using Net::Telnet
    # to connect to a non-telnet service such as SMTP.
    def telnetmode=(mode)
      if (true == mode or false == mode)
        @options["Telnetmode"] = mode
      else
        raise ArgumentError, "argument must be true or false"
      end
    end

    # Turn newline conversion on (+mode+ == false) or off (+mode+ == true),
    # or return the current value (+mode+ is not specified).
    def binmode(mode = nil)
      case mode
      when nil
        @options["Binmode"]
      when true, false
        @options["Binmode"] = mode
      else
        raise ArgumentError, "argument must be true or false"
      end
    end

    # Turn newline conversion on (false) or off (true).
    def binmode=(mode)
      if (true == mode or false == mode)
        @options["Binmode"] = mode
      else
        raise ArgumentError, "argument must be true or false"
      end
    end

    # Preprocess received data from the host.
    #
    # Performs newline conversion and detects telnet command sequences.
    # Called automatically by #waitfor().  You should only use this
    # method yourself if you have read input directly using sysread()
    # or similar, and even then only if in telnet mode.
    def preprocess(string)
      # combine CR+NULL into CR
      string = string.gsub(/#{CR}#{NULL}/no, CR) if @options["Telnetmode"]

      # combine EOL into "\n"
      string = string.gsub(/#{EOL}/no, "\n") unless @options["Binmode"]

      # remove NULL
      string = string.gsub(/#{NULL}/no, '') unless @options["Binmode"]

      string.gsub(/#{IAC}(
                   [#{IAC}#{AO}#{AYT}#{DM}#{IP}#{NOP}]|
                   [#{DO}#{DONT}#{WILL}#{WONT}]
                     [#{OPT_BINARY}-#{OPT_NEW_ENVIRON}#{OPT_EXOPL}]|
                   #{SB}[^#{IAC}]*#{IAC}#{SE}
                 )/xno) do
        if    IAC == $1  # handle escaped IAC characters
          IAC
        elsif AYT == $1  # respond to "IAC AYT" (are you there)
          self.write("nobody here but us pigeons" + EOL)
          ''
        elsif DO[0] == $1[0]  # respond to "IAC DO x"
          if OPT_BINARY[0] == $1[1]
            @telnet_option["BINARY"] = true
            self.write(IAC + WILL + OPT_BINARY)
          else
            self.write(IAC + WONT + $1[1..1])
          end
          ''
        elsif DONT[0] == $1[0]  # respond to "IAC DON'T x" with "IAC WON'T x"
          self.write(IAC + WONT + $1[1..1])
          ''
        elsif WILL[0] == $1[0]  # respond to "IAC WILL x"
          if    OPT_BINARY[0] == $1[1]
            self.write(IAC + DO + OPT_BINARY)
          elsif OPT_ECHO[0] == $1[1]
            self.write(IAC + DO + OPT_ECHO)
          elsif OPT_SGA[0]  == $1[1]
            @telnet_option["SGA"] = true
            self.write(IAC + DO + OPT_SGA)
          else
            self.write(IAC + DONT + $1[1..1])
          end
          ''
        elsif WONT[0] == $1[0]  # respond to "IAC WON'T x"
          if    OPT_ECHO[0] == $1[1]
            self.write(IAC + DONT + OPT_ECHO)
          elsif OPT_SGA[0]  == $1[1]
            @telnet_option["SGA"] = false
            self.write(IAC + DONT + OPT_SGA)
          else
            self.write(IAC + DONT + $1[1..1])
          end
          ''
        else
          ''
        end
      end
    end # preprocess

    # Read data from the host until a certain sequence is matched.
    #
    # If a block is given, the received data will be yielded as it
    # is read in (not necessarily all in one go), or nil if EOF
    # occurs before any data is received.  Whether a block is given
    # or not, all data read will be returned in a single string, or again
    # nil if EOF occurs before any data is received.  Note that
    # received data includes the matched sequence we were looking for.
    #
    # +options+ can be either a regular expression or a hash of options.
    # If a regular expression, this specifies the data to wait for.
    # If a hash, this can specify the following options:
    #
    # Match:: a regular expression, specifying the data to wait for.
    # Prompt:: as for Match; used only if Match is not specified.
    # String:: as for Match, except a string that will be converted
    #          into a regular expression.  Used only if Match and
    #          Prompt are not specified.
    # Timeout:: the number of seconds to wait for data from the host
    #           before raising a Timeout::Error.  If set to false,
    #           no timeout will occur.  If not specified, the
    #           Timeout option value specified when this instance
    #           was created will be used, or, failing that, the
    #           default value of 10 seconds.
    # Waittime:: the number of seconds to wait after matching against
    #            the input data to see if more data arrives.  If more
    #            data arrives within this time, we will judge ourselves
    #            not to have matched successfully, and will continue
    #            trying to match.  If not specified, the Waittime option
    #            value specified when this instance was created will be
    #            used, or, failing that, the default value of 0 seconds,
    #            which means not to wait for more input.
    # FailEOF:: if true, when the remote end closes the connection then an
    #           EOFError will be raised. Otherwise, defaults to the old
    #           behaviour that the function will return whatever data
    #           has been received already, or nil if nothing was received.
    #
    def waitfor(options) # :yield: recvdata
      time_out = @options["Timeout"]
      waittime = @options["Waittime"]
      fail_eof = @options["FailEOF"]

      if options.kind_of?(Hash)
        prompt   = if options.has_key?("Match")
                     options["Match"]
                   elsif options.has_key?("Prompt")
                     options["Prompt"]
                   elsif options.has_key?("String")
                     Regexp.new( Regexp.quote(options["String"]) )
                   end
        time_out = options["Timeout"]  if options.has_key?("Timeout")
        waittime = options["Waittime"] if options.has_key?("Waittime")
        fail_eof = options["FailEOF"]  if options.has_key?("FailEOF")
      else
        prompt = options
      end

      if time_out == false
        time_out = nil
      end

      line = ''
      buf = ''
      rest = ''
      until(prompt === line and not IO::select([@sock], nil, nil, waittime))
        unless IO::select([@sock], nil, nil, time_out)
          raise TimeoutError, "timed out while waiting for more data"
        end
        begin
          c = @sock.readpartial(1024 * 1024)
          @dumplog.log_dump('<', c) if @options.has_key?("Dump_log")
          if @options["Telnetmode"]
            c = rest + c
            if Integer(c.rindex(/#{IAC}#{SE}/no) || 0) <
               Integer(c.rindex(/#{IAC}#{SB}/no) || 0)
              buf = preprocess(c[0 ... c.rindex(/#{IAC}#{SB}/no)])
              rest = c[c.rindex(/#{IAC}#{SB}/no) .. -1]
            elsif pt = c.rindex(/#{IAC}[^#{IAC}#{AO}#{AYT}#{DM}#{IP}#{NOP}]?\z/no) ||
                       c.rindex(/\r\z/no)
              buf = preprocess(c[0 ... pt])
              rest = c[pt .. -1]
            else
              buf = preprocess(c)
              rest = ''
            end
         else
           # Not Telnetmode.
           #
           # We cannot use preprocess() on this data, because that
           # method makes some Telnetmode-specific assumptions.
           buf = rest + c
           rest = ''
           unless @options["Binmode"]
             if pt = buf.rindex(/\r\z/no)
               buf = buf[0 ... pt]
               rest = buf[pt .. -1]
             end
             buf.gsub!(/#{EOL}/no, "\n")
           end
          end
          @log.print(buf) if @options.has_key?("Output_log")
          line += buf
          yield buf if block_given?
        rescue EOFError # End of file reached
          raise if fail_eof
          if line == ''
            line = nil
            yield nil if block_given?
          end
          break
        end
      end
      line
    end

    # Write +string+ to the host.
    #
    # Does not perform any conversions on +string+.  Will log +string+ to the
    # dumplog, if the Dump_log option is set.
    def write(string)
      length = string.length
      while 0 < length
        IO::select(nil, [@sock])
        @dumplog.log_dump('>', string[-length..-1]) if @options.has_key?("Dump_log")
        length -= @sock.syswrite(string[-length..-1])
      end
    end

    # Sends a string to the host.
    #
    # This does _not_ automatically append a newline to the string.  Embedded
    # newlines may be converted and telnet command sequences escaped
    # depending upon the values of telnetmode, binmode, and telnet options
    # set by the host.
    def print(string)
      string = string.gsub(/#{IAC}/no, IAC + IAC) if @options["Telnetmode"]

      if @options["Binmode"]
        self.write(string)
      else
        if @telnet_option["BINARY"] and @telnet_option["SGA"]
          # IAC WILL SGA IAC DO BIN send EOL --> CR
          self.write(string.gsub(/\n/n, CR))
        elsif @telnet_option["SGA"]
          # IAC WILL SGA send EOL --> CR+NULL
          self.write(string.gsub(/\n/n, CR + NULL))
        else
          # NONE send EOL --> CR+LF
          self.write(string.gsub(/\n/n, EOL))
        end
      end
    end

    # Sends a string to the host.
    #
    # Same as #print(), but appends a newline to the string.
    def puts(string)
      self.print(string + "\n")
    end

    # Send a command to the host.
    #
    # More exactly, sends a string to the host, and reads in all received
    # data until is sees the prompt or other matched sequence.
    #
    # If a block is given, the received data will be yielded to it as
    # it is read in.  Whether a block is given or not, the received data
    # will be return as a string.  Note that the received data includes
    # the prompt and in most cases the host's echo of our command.
    #
    # +options+ is either a String, specified the string or command to
    # send to the host; or it is a hash of options.  If a hash, the
    # following options can be specified:
    #
    # String:: the command or other string to send to the host.
    # Match:: a regular expression, the sequence to look for in
    #         the received data before returning.  If not specified,
    #         the Prompt option value specified when this instance
    #         was created will be used, or, failing that, the default
    #         prompt of /[$%#>] \z/n.
    # Timeout:: the seconds to wait for data from the host before raising
    #           a Timeout error.  If not specified, the Timeout option
    #           value specified when this instance was created will be
    #           used, or, failing that, the default value of 10 seconds.
    #
    # The command or other string will have the newline sequence appended
    # to it.
    def cmd(options) # :yield: recvdata
      match    = @options["Prompt"]
      time_out = @options["Timeout"]
      fail_eof = @options["FailEOF"]

      if options.kind_of?(Hash)
        string   = options["String"]
        match    = options["Match"]   if options.has_key?("Match")
        time_out = options["Timeout"] if options.has_key?("Timeout")
        fail_eof = options["FailEOF"] if options.has_key?("FailEOF")
      else
        string = options
      end

      self.puts(string)
      if block_given?
        waitfor({"Prompt" => match, "Timeout" => time_out, "FailEOF" => fail_eof}){|c| yield c }
      else
        waitfor({"Prompt" => match, "Timeout" => time_out, "FailEOF" => fail_eof})
      end
    end

    # Login to the host with a given username and password.
    #
    # The username and password can either be provided as two string
    # arguments in that order, or as a hash with keys "Name" and
    # "Password".
    #
    # This method looks for the strings "login" and "Password" from the
    # host to determine when to send the username and password.  If the
    # login sequence does not follow this pattern (for instance, you
    # are connecting to a service other than telnet), you will need
    # to handle login yourself.
    #
    # The password can be omitted, either by only
    # provided one String argument, which will be used as the username,
    # or by providing a has that has no "Password" key.  In this case,
    # the method will not look for the "Password:" prompt; if it is
    # sent, it will have to be dealt with by later calls.
    #
    # The method returns all data received during the login process from
    # the host, including the echoed username but not the password (which
    # the host should not echo).  If a block is passed in, this received
    # data is also yielded to the block as it is received.
    def login(options, password = nil) # :yield: recvdata
      login_prompt = /[Ll]ogin[: ]*\z/n
      password_prompt = /[Pp]ass(?:word|phrase)[: ]*\z/n
      if options.kind_of?(Hash)
        username = options["Name"]
        password = options["Password"]
        login_prompt = options["LoginPrompt"] if options["LoginPrompt"]
        password_prompt = options["PasswordPrompt"] if options["PasswordPrompt"]
      else
        username = options
      end

      if block_given?
        line = waitfor(login_prompt){|c| yield c }
        if password
          line += cmd({"String" => username,
                       "Match" => password_prompt}){|c| yield c }
          line += cmd(password){|c| yield c }
        else
          line += cmd(username){|c| yield c }
        end
      else
        line = waitfor(login_prompt)
        if password
          line += cmd({"String" => username,
                       "Match" => password_prompt})
          line += cmd(password)
        else
          line += cmd(username)
        end
      end
      line
    end

    # Closes the connection
    def close
      @sock.close
    end

  end  # class Telnet
end  # module Net

