#! ./miniruby

exit if defined?(CROSS_COMPILING)
ruby = ENV["RUBY"]
unless ruby
  load './rbconfig.rb'
  ruby = "./#{RbConfig::CONFIG['ruby_install_name']}#{RbConfig::CONFIG['EXEEXT']}"
end
unless File.exist? ruby
  print "#{ruby} is not found.\n"
  print "Try `make' first, then `make test', please.\n"
  exit false
end

$stderr.reopen($stdout)
error = ''

srcdir = File.expand_path('..', File.dirname(__FILE__))
`#{ruby} #{srcdir}/sample/test.rb`.each_line do |line|
  if line =~ /^end of test/
    print "\ntest succeeded\n"
    exit true
  end
  error << line if %r:^(sample/test.rb|not): =~ line
end
puts
print error
print "test failed\n"
exit false
