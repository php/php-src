require "coverage.so"

Coverage.start

ext = ENV["COVERUBY_EXT"] || ".cov"
accum = ENV["COVERUBY_ACCUM"]
accum = !accum || accum == "" || !(%w(f n 0).include?(accum[0]))
pwd = Dir.pwd

at_exit do
  exit_exc = $!
  Dir.chdir(pwd) do
    Coverage.result.each do |sfile, covs|
      cfile = sfile + ext

      writable = proc do |f|
        File.writable?(f) || File.writable?(File.dirname(f))
      end
      unless writable[cfile]
        cfile = cfile.gsub(File.PATH_SEPARATOR, "#")
        next unless writable[cfile]
      end

      readlines = proc do |f|
        File.read(f).force_encoding("ASCII-8BIT").lines.to_a
      end

      sources = (readlines[sfile] rescue [])

      pcovs = []
      if accum
        pcovs = (readlines[cfile] rescue []).map.with_index do |line, idx|
          if line[/^\s*(?:(#####)|(\d+)|-):\s*\d+:(.*)$/n]
            cov, line = $1 ? 0 : ($2 ? $2.to_i : nil), $3
            if !sources[idx] || sources[idx].chomp != line.chomp
              warn("source file changed, ignoring: `#{ cfile }'")
              break []
            end
            cov
          else
	    p line
            warn("coverage file corrupted, ignoring: #{ cfile }")
            break []
          end
        end
        unless pcovs.empty? || pcovs.size == covs.size
          warn("coverage file changed, ignoring: `#{ cfile }'")
          pcovs = []
        end
      end

      open(cfile, "w") do |out|
        covs.zip(sources, pcovs).each_with_index do |(cov, line, pcov), idx|
          cov += pcov || 0 if cov
          cov = (cov ? (cov == 0 ? "#####" : cov.to_s) : "-").rjust(9)
          out.puts("%s:% 5d:%s" % [cov, idx + 1, line])
        end
      end
    end
  end
  raise exit_exc if exit_exc
end
