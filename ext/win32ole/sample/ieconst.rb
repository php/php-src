require 'win32ole'

ie = WIN32OLE.new('InternetExplorer.Application')
=begin
WIN32OLE.const_load(ie)
WIN32OLE.constants.sort.each do |c|
  puts "#{c} = #{WIN32OLE.const_get(c)}"
end
=end

module IE_CONST
end

WIN32OLE.const_load(ie, IE_CONST)
IE_CONST.constants.sort.each do |c|
  puts "#{c} = #{IE_CONST.const_get(c)}"
end

#------------------------------------------------------------
# Remark!!! CONSTANTS has not tested enoughly!!!
# CONSTANTS is alpha release.
# If there are constants which first letter is not [a-zA-Z],
# like a '_Foo', then maybe you can access the value by
# using CONSTANTS['_Foo']
#------------------------------------------------------------
IE_CONST::CONSTANTS.each do |k, v|
  puts "#{k} = #{v}"
end

puts WIN32OLE::VERSION
ie.quit

