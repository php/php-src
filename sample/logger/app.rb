#!/usr/bin/env ruby

require 'logger'

class MyApp < Logger::Application
  def initialize(a, b, c)
    super('MyApp')

    # Set logDevice here.
    logfile = 'app.log'
    self.log = logfile
    self.level = INFO

    # Initialize your application...
    @a = a
    @b = b
    @c = c
  end

  def run
    @log.info  { 'Started.' }

    @log.info  { "This block isn't evaled because 'debug' is not severe here." }
    @log.debug { "Result = " << foo(0) }
    @log.info  { "So nothing is dumped." }

    @log.info  { "This block is evaled because 'info' is enough severe here." }
    @log.info  { "Result = " << foo(0) }
    @log.info  { "Above causes exception, so not reached here." }

    @log.info  { 'Finished.' }
  end

private

  def foo(var)
    1 / var
  end
end

status = MyApp.new(1, 2, 3).start

if status != 0
  puts 'Some error(s) occured.'
  puts 'See "app.log".'
end
