#
# httpauth/htdigest.rb -- Apache compatible htdigest file
#
# Author: IPR -- Internet Programming with Ruby -- writers
# Copyright (c) 2003 Internet Programming with Ruby writers. All rights
# reserved.
#
# $IPR: htdigest.rb,v 1.4 2003/07/22 19:20:45 gotoyuzo Exp $

require 'webrick/httpauth/userdb'
require 'webrick/httpauth/digestauth'
require 'tempfile'

module WEBrick
  module HTTPAuth

    ##
    # Htdigest accesses apache-compatible digest password files.  Passwords are
    # matched to a realm where they are valid.  For security, the path for a
    # digest password database should be stored outside of the paths available
    # to the HTTP server.
    #
    # Htdigest is intended for use with WEBrick::HTTPAuth::DigestAuth and
    # stores passwords using cryptographic hashes.
    #
    #   htpasswd = WEBrick::HTTPAuth::Htdigest.new 'my_password_file'
    #   htpasswd.set_passwd 'my realm', 'username', 'password'
    #   htpasswd.flush

    class Htdigest
      include UserDB

      ##
      # Open a digest password database at +path+

      def initialize(path)
        @path = path
        @mtime = Time.at(0)
        @digest = Hash.new
        @mutex = Mutex::new
        @auth_type = DigestAuth
        open(@path,"a").close unless File::exist?(@path)
        reload
      end

      ##
      # Reloads passwords from the database

      def reload
        mtime = File::mtime(@path)
        if mtime > @mtime
          @digest.clear
          open(@path){|io|
            while line = io.gets
              line.chomp!
              user, realm, pass = line.split(/:/, 3)
              unless @digest[realm]
                @digest[realm] = Hash.new
              end
              @digest[realm][user] = pass
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
        if hash = @digest[realm]
          hash[user]
        end
      end

      ##
      # Sets a password in the database for +user+ in +realm+ to +pass+.

      def set_passwd(realm, user, pass)
        @mutex.synchronize{
          unless @digest[realm]
            @digest[realm] = Hash.new
          end
          @digest[realm][user] = make_passwd(realm, user, pass)
        }
      end

      ##
      # Removes a password from the database for +user+ in +realm+.

      def delete_passwd(realm, user)
        if hash = @digest[realm]
          hash.delete(user)
        end
      end

      ##
      # Iterate passwords in the database.

      def each # :yields: [user, realm, password_hash]
        @digest.keys.sort.each{|realm|
          hash = @digest[realm]
          hash.keys.sort.each{|user|
            yield([user, realm, hash[user]])
          }
        }
      end
    end
  end
end
