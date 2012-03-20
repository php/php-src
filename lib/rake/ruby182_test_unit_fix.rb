# Local Rake override to fix bug in Ruby 0.8.2
module Test                     # :nodoc:
  # Local Rake override to fix bug in Ruby 0.8.2
  module Unit                   # :nodoc:
    # Local Rake override to fix bug in Ruby 0.8.2
    module Collector            # :nodoc:
      # Local Rake override to fix bug in Ruby 0.8.2
      class Dir                 # :nodoc:
        undef collect_file
        def collect_file(name, suites, already_gathered) # :nodoc:
          dir = File.dirname(File.expand_path(name))
          $:.unshift(dir) unless $:.first == dir
          if(@req)
            @req.require(name)
          else
            require(name)
          end
          find_test_cases(already_gathered).each{|t| add_suite(suites, t.suite)}
        ensure
          $:.delete_at $:.rindex(dir)
        end
      end
    end
  end
end
