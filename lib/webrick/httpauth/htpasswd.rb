#
# httpauth/htpasswd -- Apache compatible htpasswd file
#
# Author: IPR -- Internet Programming with Ruby -- writers
# Copyright (c) 2003 Internet Programming with Ruby writers. All rights
# reserved.
#
# $IPR: htpasswd.rb,v 1.4 2003/07/22 19:20:45 gotoyuzo Exp $

require 'webrick/httpauth/userdb'
require 'webrick/httpauth/basicauth'
require 'tempfile'

module WEBrick
  module HTTPAuth

    ##
    # Htpasswd accesses apache-compatible password files.  Passwords are
    # matched to a realm where they are valid.  For security, the path for a
    # password database should be stored outside of the paths available to the
    # HTTP server.
    #
    # Htpasswd is intended for use with WEBrick::HTTPAuth::BasicAuth.
    #
    # To create an Htpasswd database with a single user:
    #
    #   htpasswd = WEBrick::HTTPAuth::Htpasswd.new 'my_password_file'
    #   htpasswd.set_passwd 'my realm', 'username', 'password'
    #   htpasswd.flush

    class Htpasswd
      include UserDB

      ##
      # Open a password database at +path+

      def initialize(path)
        @path = path
        @mtime = Time.at(0)
        @passwd = Hash.new
        @auth_type = BasicAuth
        open(@path,"a").close unless File::exist?(@path)
        reload
      end

      ##
      # Reload passwords from the database

      def reload
        mtime = File::mtime(@path)
        if mtime > @mtime
          @passwd.clear
          open(@path){|io|
            while line = io.gets
              line.chomp!
              case line
              when %r!\A[^:]+:[a-zA-Z0-9./]{13}\z!
                user, pass = line.split(":")
              when /:\$/, /:{SHA}/
                raise NotImplementedError,
                      'MD5, SHA1 .htpasswd file not supported'
              else
                raise StandardError, 'bad .htpasswd file'
              end
              @passwd[user] = pass
            end
          }
          @mtime = mtime
        end
      end

      ##
      # Flush the password database.  If +output+ is given the database will
      # be written there instead of to the original path.

      def flush(output=nil)
        output ||= @path
        tmp = Tempfile.new("htpasswd", File::dirname(output))
        begin
          each{|item| tmp.puts(item.join(":")) }
          tmp.close
          File::rename(tmp.path, output)
        rescue
          tmp.close(true)
        end
      end

      ##
      # Retrieves a password from the database for +user+ in +realm+.  If
      # +reload_db+ is true the database will be reloaded first.

      def get_passwd(realm, user, reload_db)
        reload() if reload_db
        @passwd[user]
      end

      ##
      # Sets a password in the database for +user+ in +realm+ to +pass+.

      def set_passwd(realm, user, pass)
        @passwd[user] = make_passwd(realm, user, pass)
      end

      ##
      # Removes a password from the database for +user+ in +realm+.

      def delete_passwd(realm, user)
        @passwd.delete(user)
      end

      ##
      # Iterate passwords in the database.

      def each # :yields: [user, password]
        @passwd.keys.sort.each{|user|
          yield([user, @passwd[user]])
        }
      end
    end
  end
end
