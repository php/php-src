require 'rake'

# Load the test files from the command line.
argv = ARGV.select do |argument|
  case argument
  when /^-/ then
    argument
  when /\*/ then
    FileList[argument].to_a.each do |file|
      require File.expand_path file
    end

    false
  else
    require File.expand_path argument

    false
  end
end

ARGV.replace argv

