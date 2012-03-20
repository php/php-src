#
# filehandler.rb -- FileHandler Module
#
# Author: IPR -- Internet Programming with Ruby -- writers
# Copyright (c) 2001 TAKAHASHI Masayoshi, GOTOU Yuuzou
# Copyright (c) 2003 Internet Programming with Ruby writers. All rights
# reserved.
#
# $IPR: filehandler.rb,v 1.44 2003/06/07 01:34:51 gotoyuzo Exp $

require 'thread'
require 'time'

require 'webrick/htmlutils'
require 'webrick/httputils'
require 'webrick/httpstatus'

module WEBrick
  module HTTPServlet

    class DefaultFileHandler < AbstractServlet
      def initialize(server, local_path)
        super(server, local_path)
        @local_path = local_path
      end

      def do_GET(req, res)
        st = File::stat(@local_path)
        mtime = st.mtime
        res['etag'] = sprintf("%x-%x-%x", st.ino, st.size, st.mtime.to_i)

        if not_modified?(req, res, mtime, res['etag'])
          res.body = ''
          raise HTTPStatus::NotModified
        elsif req['range']
          make_partial_content(req, res, @local_path, st.size)
          raise HTTPStatus::PartialContent
        else
          mtype = HTTPUtils::mime_type(@local_path, @config[:MimeTypes])
          res['content-type'] = mtype
          res['content-length'] = st.size
          res['last-modified'] = mtime.httpdate
          res.body = open(@local_path, "rb")
        end
      end

      def not_modified?(req, res, mtime, etag)
        if ir = req['if-range']
          begin
            if Time.httpdate(ir) >= mtime
              return true
            end
          rescue
            if HTTPUtils::split_header_value(ir).member?(res['etag'])
              return true
            end
          end
        end

        if (ims = req['if-modified-since']) && Time.parse(ims) >= mtime
          return true
        end

        if (inm = req['if-none-match']) &&
           HTTPUtils::split_header_value(inm).member?(res['etag'])
          return true
        end

        return false
      end

      def make_partial_content(req, res, filename, filesize)
        mtype = HTTPUtils::mime_type(filename, @config[:MimeTypes])
        unless ranges = HTTPUtils::parse_range_header(req['range'])
          raise HTTPStatus::BadRequest,
            "Unrecognized range-spec: \"#{req['range']}\""
        end
        open(filename, "rb"){|io|
          if ranges.size > 1
            time = Time.now
            boundary = "#{time.sec}_#{time.usec}_#{Process::pid}"
            body = ''
            ranges.each{|range|
              first, last = prepare_range(range, filesize)
              next if first < 0
              io.pos = first
              content = io.read(last-first+1)
              body << "--" << boundary << CRLF
              body << "Content-Type: #{mtype}" << CRLF
              body << "Content-Range: bytes #{first}-#{last}/#{filesize}" << CRLF
              body << CRLF
              body << content
              body << CRLF
            }
            raise HTTPStatus::RequestRangeNotSatisfiable if body.empty?
            body << "--" << boundary << "--" << CRLF
            res["content-type"] = "multipart/byteranges; boundary=#{boundary}"
            res.body = body
          elsif range = ranges[0]
            first, last = prepare_range(range, filesize)
            raise HTTPStatus::RequestRangeNotSatisfiable if first < 0
            if last == filesize - 1
              content = io.dup
              content.pos = first
            else
              io.pos = first
              content = io.read(last-first+1)
            end
            res['content-type'] = mtype
            res['content-range'] = "bytes #{first}-#{last}/#{filesize}"
            res['content-length'] = last - first + 1
            res.body = content
          else
            raise HTTPStatus::BadRequest
          end
        }
      end

      def prepare_range(range, filesize)
        first = range.first < 0 ? filesize + range.first : range.first
        return -1, -1 if first < 0 || first >= filesize
        last = range.last < 0 ? filesize + range.last : range.last
        last = filesize - 1 if last >= filesize
        return first, last
      end
    end

    ##
    # Serves files from a directory

    class FileHandler < AbstractServlet
      HandlerTable = Hash.new

      ##
      # Allow custom handling of requests for files with +suffix+ by class
      # +handler+

      def self.add_handler(suffix, handler)
        HandlerTable[suffix] = handler
      end

      ##
      # Remove custom handling of requests for files with +suffix+

      def self.remove_handler(suffix)
        HandlerTable.delete(suffix)
      end

      ##
      # Creates a FileHandler servlet on +server+ that serves files starting
      # at directory +root+
      #
      # If +options+ is a Hash the following keys are allowed:
      #
      # :AcceptableLanguages:: Array of languages allowed for accept-language
      # :DirectoryCallback:: Allows preprocessing of directory requests
      # :FancyIndexing:: If true, show an index for directories
      # :FileCallback:: Allows preprocessing of file requests
      # :HandlerCallback:: Allows preprocessing of requests
      # :HandlerTable:: Maps file suffixes to file handlers.
      #                 DefaultFileHandler is used by default but any servlet
      #                 can be used.
      # :NondisclosureName:: Do not show files matching this array of globs
      # :UserDir:: Directory inside ~user to serve content from for /~user
      #            requests.  Only works if mounted on /
      #
      # If +options+ is true or false then +:FancyIndexing+ is enabled or
      # disabled respectively.

      def initialize(server, root, options={}, default=Config::FileHandler)
        @config = server.config
        @logger = @config[:Logger]
        @root = File.expand_path(root)
        if options == true || options == false
          options = { :FancyIndexing => options }
        end
        @options = default.dup.update(options)
      end

      def service(req, res)
        # if this class is mounted on "/" and /~username is requested.
        # we're going to override path informations before invoking service.
        if defined?(Etc) && @options[:UserDir] && req.script_name.empty?
          if %r|^(/~([^/]+))| =~ req.path_info
            script_name, user = $1, $2
            path_info = $'
            begin
              passwd = Etc::getpwnam(user)
              @root = File::join(passwd.dir, @options[:UserDir])
              req.script_name = script_name
              req.path_info = path_info
            rescue
              @logger.debug "#{self.class}#do_GET: getpwnam(#{user}) failed"
            end
          end
        end
        prevent_directory_traversal(req, res)
        super(req, res)
      end

      def do_GET(req, res)
        unless exec_handler(req, res)
          set_dir_list(req, res)
        end
      end

      def do_POST(req, res)
        unless exec_handler(req, res)
          raise HTTPStatus::NotFound, "`#{req.path}' not found."
        end
      end

      def do_OPTIONS(req, res)
        unless exec_handler(req, res)
          super(req, res)
        end
      end

      # ToDo
      # RFC2518: HTTP Extensions for Distributed Authoring -- WEBDAV
      #
      # PROPFIND PROPPATCH MKCOL DELETE PUT COPY MOVE
      # LOCK UNLOCK

      # RFC3253: Versioning Extensions to WebDAV
      #          (Web Distributed Authoring and Versioning)
      #
      # VERSION-CONTROL REPORT CHECKOUT CHECK_IN UNCHECKOUT
      # MKWORKSPACE UPDATE LABEL MERGE ACTIVITY

      private

      def trailing_pathsep?(path)
        # check for trailing path separator:
        #   File.dirname("/aaaa/bbbb/")      #=> "/aaaa")
        #   File.dirname("/aaaa/bbbb/x")     #=> "/aaaa/bbbb")
        #   File.dirname("/aaaa/bbbb")       #=> "/aaaa")
        #   File.dirname("/aaaa/bbbbx")      #=> "/aaaa")
        return File.dirname(path) != File.dirname(path+"x")
      end

      def prevent_directory_traversal(req, res)
        # Preventing directory traversal on Windows platforms;
        # Backslashes (0x5c) in path_info are not interpreted as special
        # character in URI notation. So the value of path_info should be
        # normalize before accessing to the filesystem.

        # dirty hack for filesystem encoding; in nature, File.expand_path
        # should not be used for path normalization.  [Bug #3345]
        path = req.path_info.dup.force_encoding(Encoding.find("filesystem"))
        if trailing_pathsep?(req.path_info)
          # File.expand_path removes the trailing path separator.
          # Adding a character is a workaround to save it.
          #  File.expand_path("/aaa/")        #=> "/aaa"
          #  File.expand_path("/aaa/" + "x")  #=> "/aaa/x"
          expanded = File.expand_path(path + "x")
          expanded.chop!  # remove trailing "x"
        else
          expanded = File.expand_path(path)
        end
        expanded.force_encoding(req.path_info.encoding)
        req.path_info = expanded
      end

      def exec_handler(req, res)
        raise HTTPStatus::NotFound, "`#{req.path}' not found" unless @root
        if set_filename(req, res)
          handler = get_handler(req, res)
          call_callback(:HandlerCallback, req, res)
          h = handler.get_instance(@config, res.filename)
          h.service(req, res)
          return true
        end
        call_callback(:HandlerCallback, req, res)
        return false
      end

      def get_handler(req, res)
        suffix1 = (/\.(\w+)\z/ =~ res.filename) && $1.downcase
        if /\.(\w+)\.([\w\-]+)\z/ =~ res.filename
          if @options[:AcceptableLanguages].include?($2.downcase)
            suffix2 = $1.downcase
          end
        end
        handler_table = @options[:HandlerTable]
        return handler_table[suffix1] || handler_table[suffix2] ||
               HandlerTable[suffix1] || HandlerTable[suffix2] ||
               DefaultFileHandler
      end

      def set_filename(req, res)
        res.filename = @root.dup
        path_info = req.path_info.scan(%r|/[^/]*|)

        path_info.unshift("")  # dummy for checking @root dir
        while base = path_info.first
          break if base == "/"
          break unless File.directory?(File.expand_path(res.filename + base))
          shift_path_info(req, res, path_info)
          call_callback(:DirectoryCallback, req, res)
        end

        if base = path_info.first
          if base == "/"
            if file = search_index_file(req, res)
              shift_path_info(req, res, path_info, file)
              call_callback(:FileCallback, req, res)
              return true
            end
            shift_path_info(req, res, path_info)
          elsif file = search_file(req, res, base)
            shift_path_info(req, res, path_info, file)
            call_callback(:FileCallback, req, res)
            return true
          else
            raise HTTPStatus::NotFound, "`#{req.path}' not found."
          end
        end

        return false
      end

      def check_filename(req, res, name)
        if nondisclosure_name?(name) || windows_ambiguous_name?(name)
          @logger.warn("the request refers nondisclosure name `#{name}'.")
          raise HTTPStatus::NotFound, "`#{req.path}' not found."
        end
      end

      def shift_path_info(req, res, path_info, base=nil)
        tmp = path_info.shift
        base = base || tmp
        req.path_info = path_info.join
        req.script_name << base
        res.filename = File.expand_path(res.filename + base)
        check_filename(req, res, File.basename(res.filename))
      end

      def search_index_file(req, res)
        @config[:DirectoryIndex].each{|index|
          if file = search_file(req, res, "/"+index)
            return file
          end
        }
        return nil
      end

      def search_file(req, res, basename)
        langs = @options[:AcceptableLanguages]
        path = res.filename + basename
        if File.file?(path)
          return basename
        elsif langs.size > 0
          req.accept_language.each{|lang|
            path_with_lang = path + ".#{lang}"
            if langs.member?(lang) && File.file?(path_with_lang)
              return basename + ".#{lang}"
            end
          }
          (langs - req.accept_language).each{|lang|
            path_with_lang = path + ".#{lang}"
            if File.file?(path_with_lang)
              return basename + ".#{lang}"
            end
          }
        end
        return nil
      end

      def call_callback(callback_name, req, res)
        if cb = @options[callback_name]
          cb.call(req, res)
        end
      end

      def windows_ambiguous_name?(name)
        return true if /[. ]+\z/ =~ name
        return true if /::\$DATA\z/ =~ name
        return false
      end

      def nondisclosure_name?(name)
        @options[:NondisclosureName].each{|pattern|
          if File.fnmatch(pattern, name, File::FNM_CASEFOLD)
            return true
          end
        }
        return false
      end

      def set_dir_list(req, res)
        redirect_to_directory_uri(req, res)
        unless @options[:FancyIndexing]
          raise HTTPStatus::Forbidden, "no access permission to `#{req.path}'"
        end
        local_path = res.filename
        list = Dir::entries(local_path).collect{|name|
          next if name == "." || name == ".."
          next if nondisclosure_name?(name)
          next if windows_ambiguous_name?(name)
          st = (File::stat(File.join(local_path, name)) rescue nil)
          if st.nil?
            [ name, nil, -1 ]
          elsif st.directory?
            [ name + "/", st.mtime, -1 ]
          else
            [ name, st.mtime, st.size ]
          end
        }
        list.compact!

        if    d0 = req.query["N"]; idx = 0
        elsif d0 = req.query["M"]; idx = 1
        elsif d0 = req.query["S"]; idx = 2
        else  d0 = "A"           ; idx = 0
        end
        d1 = (d0 == "A") ? "D" : "A"

        if d0 == "A"
          list.sort!{|a,b| a[idx] <=> b[idx] }
        else
          list.sort!{|a,b| b[idx] <=> a[idx] }
        end

        res['content-type'] = "text/html"

        res.body = <<-_end_of_html_
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">
<HTML>
  <HEAD><TITLE>Index of #{HTMLUtils::escape(req.path)}</TITLE></HEAD>
  <BODY>
    <H1>Index of #{HTMLUtils::escape(req.path)}</H1>
        _end_of_html_

        res.body << "<PRE>\n"
        res.body << " <A HREF=\"?N=#{d1}\">Name</A>                          "
        res.body << "<A HREF=\"?M=#{d1}\">Last modified</A>         "
        res.body << "<A HREF=\"?S=#{d1}\">Size</A>\n"
        res.body << "<HR>\n"

        list.unshift [ "..", File::mtime(local_path+"/.."), -1 ]
        list.each{ |name, time, size|
          if name == ".."
            dname = "Parent Directory"
          elsif name.bytesize > 25
            dname = name.sub(/^(.{23})(?:.*)/, '\1..')
          else
            dname = name
          end
          s =  " <A HREF=\"#{HTTPUtils::escape(name)}\">#{HTMLUtils::escape(dname)}</A>"
          s << " " * (30 - dname.bytesize)
          s << (time ? time.strftime("%Y/%m/%d %H:%M      ") : " " * 22)
          s << (size >= 0 ? size.to_s : "-") << "\n"
          res.body << s
        }
        res.body << "</PRE><HR>"

        res.body << <<-_end_of_html_
    <ADDRESS>
     #{HTMLUtils::escape(@config[:ServerSoftware])}<BR>
     at #{req.host}:#{req.port}
    </ADDRESS>
  </BODY>
</HTML>
        _end_of_html_
      end

    end
  end
end
