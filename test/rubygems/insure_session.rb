require 'rubygems'

def install_session
  path_to_gem = File.join("redist", "session.gem")
  begin
    Gem::Installer.new(path_to_gem).install
  rescue Errno::EACCES => ex
    puts
    puts "*****************************************************************"
    puts "Unable to install Gem 'Session'."
    puts "Reason:  #{ex.message}"
    puts "Try running:"
    puts
    puts "   gem -Li #{path_to_gem}"
    puts
    puts "with the appropriate admin privileges."
    puts "*****************************************************************"
    puts
    exit
  end
  gem 'session'
end

begin
  require 'session'
rescue LoadError => e
  puts
  puts "Required Gem 'Session' missing."
  puts "We can attempt to install from the RubyGems Distribution,"
  puts "but installation may require admin privileges on your system."
  puts
  print "Install now from RubyGems distribution? [Yn]"
  answer = gets
  if(answer =~ /^y/i || answer =~ /^[^a-zA-Z0-9]$/) then
    install_session
    puts
    puts "Retry running the functional tests."
    exit(0)
  else
    puts "Test cancelled...quitting"
    exit(1)
  end
end
