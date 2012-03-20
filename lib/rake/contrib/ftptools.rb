# = Tools for FTP uploading.
#
# This file is still under development and is not released for general
# use.

require 'date'
require 'net/ftp'

module Rake # :nodoc:

  ####################################################################
  # <b>Note:</b> <em> Not released for general use.</em>
  class FtpFile
    attr_reader :name, :size, :owner, :group, :time

    def self.date
      @date_class ||= Date
    end

    def self.time
      @time_class ||= Time
    end

    def initialize(path, entry)
      @path = path
      @mode, _, @owner, @group, size, d1, d2, d3, @name = entry.split(' ')
      @size = size.to_i
      @time = determine_time(d1, d2, d3)
    end

    def path
      File.join(@path, @name)
    end

    def directory?
      @mode[0] == ?d
    end

    def mode
      parse_mode(@mode)
    end

    def symlink?
      @mode[0] == ?l
    end

    private # --------------------------------------------------------

    def parse_mode(m)
      result = 0
      (1..9).each do |i|
        result = 2*result + ((m[i]==?-) ? 0 : 1)
      end
      result
    end

    def determine_time(d1, d2, d3)
      now = self.class.time.now
      if /:/ =~ d3
        result = Time.parse("#{d1} #{d2} #{now.year} #{d3}")
        if result > now
          result = Time.parse("#{d1} #{d2} #{now.year-1} #{d3}")
        end
      else
        result = Time.parse("#{d1} #{d2} #{d3}")
      end
      result
#       elements = ParseDate.parsedate("#{d1} #{d2} #{d3}")
#       if elements[0].nil?
#         today = self.class.date.today
#         if elements[1] > today.month
#           elements[0] = today.year - 1
#         else
#           elements[0] = today.year
#         end
#       end
#       elements = elements.collect { |el| el.nil? ? 0 : el }
#       Time.mktime(*elements[0,7])
    end
  end

  ####################################################################
  # Manage the uploading of files to an FTP account.
  class FtpUploader

    # Log uploads to standard output when true.
    attr_accessor :verbose

    class << FtpUploader
      # Create an uploader and pass it to the given block as +up+.
      # When the block is complete, close the uploader.
      def connect(path, host, account, password)
        up = self.new(path, host, account, password)
        begin
          yield(up)
        ensure
          up.close
        end
      end
    end

    # Create an FTP uploader targeting the directory +path+ on +host+
    # using the given account and password.  +path+ will be the root
    # path of the uploader.
    def initialize(path, host, account, password)
      @created = Hash.new
      @path = path
      @ftp = Net::FTP.new(host, account, password)
      makedirs(@path)
      @ftp.chdir(@path)
    end

    # Create the directory +path+ in the uploader root path.
    def makedirs(path)
      route = []
      File.split(path).each do |dir|
        route << dir
        current_dir = File.join(route)
        if @created[current_dir].nil?
          @created[current_dir] = true
          $stderr.puts "Creating Directory  #{current_dir}" if @verbose
          @ftp.mkdir(current_dir) rescue nil
        end
      end
    end

    # Upload all files matching +wildcard+ to the uploader's root
    # path.
    def upload_files(wildcard)
      Dir[wildcard].each do |fn|
        upload(fn)
      end
    end

    # Close the uploader.
    def close
      @ftp.close
    end

    private # --------------------------------------------------------

    # Upload a single file to the uploader's root path.
    def upload(file)
      $stderr.puts "Uploading #{file}" if @verbose
      dir = File.dirname(file)
      makedirs(dir)
      @ftp.putbinaryfile(file, file) unless File.directory?(file)
    end
  end
end
