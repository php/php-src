require_relative 'inlinetest.rb'
target = __FILE__[/test_(.*\.rb)$/, 1]
InlineTest.loadtest__END__part(target)
