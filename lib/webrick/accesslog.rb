#--
# accesslog.rb -- Access log handling utilities
#
# Author: IPR -- Internet Programming with Ruby -- writers
# Copyright (c) 2002 keita yamaguchi
# Copyright (c) 2002 Internet Programming with Ruby writers
#
# $IPR: accesslog.rb,v 1.1 2002/10/01 17:16:32 gotoyuzo Exp $

module WEBrick

  ##
  # AccessLog provides logging to various files in various formats.
  #
  # Multiple logs may be written to at the same time:
  #
  #   access_log = [
  #     [$stderr, WEBrick::AccessLog::COMMON_LOG_FORMAT],
  #     [$stderr, WEBrick::AccessLog::REFERER_LOG_FORMAT],
  #   ]
  #
  #   server = WEBrick::HTTPServer.new :AccessLog => access_log
  #
  # Custom log formats may be defined.  WEBrick::AccessLog provides a subset
  # of the formatting from Apache's mod_log_config
  # http://httpd.apache.org/docs/mod/mod_log_config.html#formats.  See
  # AccessLog::setup_params for a list of supported options

  module AccessLog

    ##
    # Raised if a parameter such as %e, %i, %o or %n is used without fetching
    # a specific field.

    class AccessLogError < StandardError; end

    ##
    # The Common Log Format's time format

    CLF_TIME_FORMAT     = "[%d/%b/%Y:%H:%M:%S %Z]"

    ##
    # Common Log Format

    COMMON_LOG_FORMAT   = "%h %l %u %t \"%r\" %s %b"

    ##
    # Short alias for Common Log Format

    CLF                 = COMMON_LOG_FORMAT

    ##
    # Referer Log Format

    REFERER_LOG_FORMAT  = "%{Referer}i -> %U"

    ##
    # User-Agent Log Format

    AGENT_LOG_FORMAT    = "%{User-Agent}i"

    ##
    # Combined Log Format

    COMBINED_LOG_FORMAT = "#{CLF} \"%{Referer}i\" \"%{User-agent}i\""

    module_function

    # This format specification is a subset of mod_log_config of Apache:
    #
    # %a:: Remote IP address
    # %b:: Total response size
    # %e{variable}:: Given variable in ENV
    # %f:: Response filename
    # %h:: Remote host name
    # %{header}i:: Given request header
    # %l:: Remote logname, always "-"
    # %m:: Request method
    # %{attr}n:: Given request attribute from <tt>req.attributes</tt>
    # %{header}o:: Given response header
    # %p:: Server's request port
    # %{format}p:: The canonical port of the server serving the request or the
    #              actual port or the client's actual port.  Valid formats are
    #              canonical, local or remote.
    # %q:: Request query string
    # %r:: First line of the request
    # %s:: Request status
    # %t:: Time the request was recieved
    # %T:: Time taken to process the request
    # %u:: Remote user from auth
    # %U:: Unparsed URI
    # %%:: Literal %

    def setup_params(config, req, res)
      params = Hash.new("")
      params["a"] = req.peeraddr[3]
      params["b"] = res.sent_size
      params["e"] = ENV
      params["f"] = res.filename || ""
      params["h"] = req.peeraddr[2]
      params["i"] = req
      params["l"] = "-"
      params["m"] = req.request_method
      params["n"] = req.attributes
      params["o"] = res
      params["p"] = req.port
      params["q"] = req.query_string
      params["r"] = req.request_line.sub(/\x0d?\x0a\z/o, '')
      params["s"] = res.status       # won't support "%>s"
      params["t"] = req.request_time
      params["T"] = Time.now - req.request_time
      params["u"] = req.user || "-"
      params["U"] = req.unparsed_uri
      params["v"] = config[:ServerName]
      params
    end

    def format(format_string, params)
      format_string.gsub(/\%(?:\{(.*?)\})?>?([a-zA-Z%])/){
         param, spec = $1, $2
         case spec[0]
         when ?e, ?i, ?n, ?o
           raise AccessLogError,
             "parameter is required for \"#{spec}\"" unless param
           (param = params[spec][param]) ? escape(param) : "-"
         when ?t
           params[spec].strftime(param || CLF_TIME_FORMAT)
         when ?p
           case param
           when 'remote'
             escape(params["i"].peeraddr[1].to_s)
           else
             escape(params["p"].to_s)
           end
         when ?%
           "%"
         else
           escape(params[spec].to_s)
         end
      }
    end

    def escape(data)
      if data.tainted?
        data.gsub(/[[:cntrl:]\\]+/) {$&.dump[1...-1]}.untaint
      else
        data
      end
    end
  end
end
