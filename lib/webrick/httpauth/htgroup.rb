#
# httpauth/htgroup.rb -- Apache compatible htgroup file
#
# Author: IPR -- Internet Programming with Ruby -- writers
# Copyright (c) 2003 Internet Programming with Ruby writers. All rights
# reserved.
#
# $IPR: htgroup.rb,v 1.1 2003/02/16 22:22:56 gotoyuzo Exp $

require 'tempfile'

module WEBrick
  module HTTPAuth

    ##
    # Htgroup accesses apache-compatible group files.  Htgroup can be used to
    # provide group-based authentication for users.  Currently Htgroup is not
    # directly integrated with any authenticators in WEBrick.  For security,
    # the path for a digest password database should be stored outside of the
    # paths available to the HTTP server.
    #
    # Example:
    #
    #   htgroup = WEBrick::HTTPAuth::Htgroup.new 'my_group_file'
    #   htgroup.add 'superheroes', %w[spiderman batman]
    #
    #   htgroup.members('superheroes').include? 'magneto' # => false

    class Htgroup

      ##
      # Open a group database at +path+

      def initialize(path)
        @path = path
        @mtime = Time.at(0)
        @group = Hash.new
        open(@path,"a").close unless File::exist?(@path)
        reload
      end

      ##
      # Reload groups from the database

      def reload
        if (mtime = File::mtime(@path)) > @mtime
          @group.clear
          open(@path){|io|
            while line = io.gets
              line.chomp!
              group, members = line.split(/:\s*/)
              @group[group] = members.split(/\s+/)
            end
          }
          @mtime = mtime
        end
      end

      ##
      # Flush the group database.  If +output+ is given the database will be
      # written there instead of to the original path.

      def flush(output=nil)
        output ||= @path
        tmp = Tempfile.new("htgroup", File::dirname(output))
        begin
          @group.keys.sort.each{|group|
            tmp.puts(format("%s: %s", group, self.members(group).join(" ")))
          }
          tmp.close
          File::rename(tmp.path, output)
        rescue
          tmp.close(true)
        end
      end

      ##
      # Retrieve the list of members from +group+

      def members(group)
        reload
        @group[group] || []
      end

      ##
      # Add an Array of +members+ to +group+

      def add(group, members)
        @group[group] = members(group) | members
      end
    end
  end
end
