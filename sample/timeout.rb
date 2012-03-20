require 'timeout'

def progress(n = 5)
  n.times {|i| print i; STDOUT.flush; sleep 1; i+= 1}
  puts "never reach"
end

p timeout(5) {
  45
}
p timeout(5, TimeoutError) {
  45
}
p timeout(nil) {
  54
}
p timeout(0) {
  54
}
begin
  timeout(5) {progress}
rescue => e
  puts e.message
end
begin
  timeout(3) {
    begin
      timeout(5) {progress}
    rescue => e
      puts "never reach"
    end
  }
rescue => e
  puts e.message
end
class MyTimeout < StandardError
end
begin
  timeout(2, MyTimeout) {progress}
rescue MyTimeout => e
  puts e.message
end
