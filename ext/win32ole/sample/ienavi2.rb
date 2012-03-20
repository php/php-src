require 'win32ole'

class IEHandler
  attr_reader :loop
  def initialize
    @urls = []
    @loop = true
  end
  def method_missing(event, *args)
    case event
    when "BeforeNavigate2"
      puts "Now Navigate #{args[1]}..."
    end
  end
  def onNavigateComplete2(pdisp, url)
    @urls << url
  end
  def onOnQuit
    puts "Now Stop IE..."
    @loop = false
  end
  def put_urls
    puts "You Navigated the URLs ..."
    @urls.each_with_index do |url, i|
      puts "(#{i+1}) #{url}"
    end
  end
end

ie = WIN32OLE.new('InternetExplorer.Application')
ie.visible = true
ie.gohome

ev = WIN32OLE_EVENT.new(ie)
ev.handler = IEHandler.new

while (ev.handler.loop)
  WIN32OLE_EVENT.message_loop
end
ev.handler.put_urls
