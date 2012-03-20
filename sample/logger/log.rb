#!/usr/bin/env ruby

require 'logger'

log = Logger.new(STDERR)

def do_log(log)
  log.debug('do_log1') { "debug" }
  log.info('do_log2') { "info" }
  log.warn('do_log3') { "warn" }
  log.error('do_log4') { "error" }
  log.fatal('do_log6') { "fatal" }
  log.unknown('do_log7') { "unknown" }
end

log.level = Logger::DEBUG	# Default.
do_log(log)

puts "Set severity threshold 'WARN'."

log.level = Logger::WARN
do_log(log)

puts "Change datetime format.  Thanks to Daniel Berger."

log.datetime_format = "%d-%b-%Y@%H:%M:%S"
do_log(log)
