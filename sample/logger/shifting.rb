#!/usr/bin/env ruby

require 'logger'

logfile = 'shifting.log'
# Max 3 age ... logShifting.log, logShifting.log.0, and logShifting.log.1
shift_age = 3
# Shift log file about for each 1024 bytes.
shift_size = 1024

log = Logger.new(logfile, shift_age, shift_size)

def do_log(log)
  log.debug('do_log1') { 'd' * rand(100) }
  log.info('do_log2') { 'i' * rand(100) }
  log.warn('do_log3') { 'w' * rand(100) }
  log.error('do_log4') { 'e' * rand(100) }
  log.fatal('do_log5') { 'f' * rand(100) }
  log.unknown('do_log6') { 'u' * rand(100) }
end

(1..10).each do
  do_log(log)
end

puts 'See shifting.log and shifting.log.[01].'
