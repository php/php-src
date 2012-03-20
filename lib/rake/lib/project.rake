task "create:project" => ["lib", "test", "Rakefile"]

directory "lib"
directory "test"

file "Rakefile" do
  File.open("Rakefile", "w") do |out|
    out.puts %{# -*- ruby -*-

require 'rake/clean'
require 'rake/testtask'

task :default => :test

Rake::TestTask.new do |t|
  t.verbose = false
  t.test_files = FileList['test/test_*.rb']
end
}
  end
end
