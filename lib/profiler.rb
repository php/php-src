# Profile provides a way to Profile your Ruby application.
#
# Profiling your program is a way of determining which methods are called and
# how long each method takes to complete.  This way you can detect which
# methods are possible bottlenecks.
#
# Profiling your program will slow down your execution time considerably,
# so activate it only when you need it.  Don't confuse benchmarking with
# profiling.
#
# There are two ways to activate Profiling:
#
# == Command line
#
# Run your Ruby script with <code>-rprofile</code>:
#
#   ruby -rprofile example.rb
#
# If you're profiling an executable in your <code>$PATH</code> you can use
# <code>ruby -S</code>:
#
#   ruby -rprofile -S some_executable
#
# == From code
#
# Just require 'profile':
#
#   require 'profile'
#
#   def slow_method
#     5000.times do
#       9999999999999999*999999999
#     end
#   end
#
#   def fast_method
#     5000.times do
#       9999999999999999+999999999
#     end
#   end
#
#   slow_method
#   fast_method
#
# The output in both cases is a report when the execution is over:
#
#   ruby -rprofile example.rb
#
#     %   cumulative   self              self     total
#    time   seconds   seconds    calls  ms/call  ms/call  name
#    68.42     0.13      0.13        2    65.00    95.00  Integer#times
#    15.79     0.16      0.03     5000     0.01     0.01  Fixnum#*
#    15.79     0.19      0.03     5000     0.01     0.01  Fixnum#+
#     0.00     0.19      0.00        2     0.00     0.00  IO#set_encoding
#     0.00     0.19      0.00        1     0.00   100.00  Object#slow_method
#     0.00     0.19      0.00        2     0.00     0.00  Module#method_added
#     0.00     0.19      0.00        1     0.00    90.00  Object#fast_method
#     0.00     0.19      0.00        1     0.00   190.00  #toplevel

module Profiler__
  # internal values
  @@start = @@stack = @@map = @@array = nil
  PROFILE_PROC = proc{|event, file, line, id, binding, klass|
    case event
    when "call", "c-call"
      now = Process.times[0]
      @@stack.push [now, 0.0]
    when "return", "c-return"
      now = Process.times[0]
      key = [klass, id]
      if tick = @@stack.pop
        data = begin
                 @@map[key] ||= [0, 0.0, 0.0, key]
               rescue NoMethodError
                 @@array.find{|i| i[3] == key} || (@@array << [0, 0.0, 0.0, key])[-1]
               end
        data[0] += 1
        cost = now - tick[0]
        data[1] += cost
        data[2] += cost - tick[1]
        @@stack[-1][1] += cost if @@stack[-1]
      end
    end
  }
module_function
  def start_profile
    @@start = Process.times[0]
    @@stack = []
    @@map = {}
    @@array = []
    set_trace_func PROFILE_PROC
  end
  def stop_profile
    set_trace_func nil
  end
  def print_profile(f)
    stop_profile
    total = Process.times[0] - @@start
    if total == 0 then total = 0.01 end
    data = @@map.values + @@array
    data = data.sort_by{|x| -x[2]}
    sum = 0
    f.printf "  %%   cumulative   self              self     total\n"
    f.printf " time   seconds   seconds    calls  ms/call  ms/call  name\n"
    for d in data
      sum += d[2]
      f.printf "%6.2f %8.2f  %8.2f %8d ", d[2]/total*100, sum, d[2], d[0]
      f.printf "%8.2f %8.2f  %s\n", d[2]*1000/d[0], d[1]*1000/d[0], get_name(*d[3])
    end
    f.printf "%6.2f %8.2f  %8.2f %8d ", 0.0, total, 0.0, 1     # ???
    f.printf "%8.2f %8.2f  %s\n", 0.0, total*1000, "#toplevel" # ???
  end
  def get_name(klass, id)
    name = klass.to_s || ""
    if klass.kind_of? Class
      name += "#"
    else
      name += "."
    end
    name + id.id2name
  rescue NoMethodError => e
    name = e.message.slice(/#<.*?:0x[0-9a-f]+>/) || ""
    name + "." + id.id2name
  end
  private :get_name
end
