require 'win32ole'

$urls = []

def navigate(url)
  $urls << url
end

def stop_msg_loop
  puts "Now Stop IE..."
  $LOOP = FALSE;
end

def default_handler(event, *args)
  case event
  when "BeforeNavigate"
    puts "Now Navigate #{args[0]}..."
  end
end

ie = WIN32OLE.new('InternetExplorer.Application')
ie.visible = TRUE
ie.gohome

ev = WIN32OLE_EVENT.new(ie, 'DWebBrowserEvents')

ev.on_event {|*args| default_handler(*args)}
ev.on_event("NavigateComplete") {|url| navigate(url)}
ev.on_event("Quit") {|*args| stop_msg_loop}

$LOOP = TRUE
while ($LOOP)
  WIN32OLE_EVENT.message_loop
end

puts "You Navigated the URLs ..."
$urls.each_with_index do |url, i|
  puts "(#{i+1}) #{url}"
end

