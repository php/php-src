stat = {}

while line = ARGF.gets
  if /\[start\] (\w+)/ =~ line
    name = $1
    puts '--------------------------------------------------------------'
    puts line
    size = 0
    len  = 0

    while line = ARGF.gets
      if /\[start\] (\w+)/ =~ line
        puts "\t; # length: #{len}, size: #{size}"
        puts "\t; # !!"
        stat[name] = [len, size]
        #
        name = $1
        puts '--------------------------------------------------------------'
        puts line
        size = 0
        len  = 0
        next
      end

      unless /(\ALM)|(\ALB)|(\A\.)|(\A\/)/ =~ line
        puts line
        if /\[length = (\d+)\]/ =~ line
          len  += $1.to_i
          size += 1
        end
      end


      if /__NEXT_INSN__/ !~ line && /\[end  \] (\w+)/ =~ line
        ename = $1
        if name != ename
          puts "!! start with #{name}, but end with #{ename}"
        end
        stat[ename] = [len, size]
        puts "\t; # length: #{len}, size: #{size}"
        break
      end
    end
  end
end

stat.sort_by{|a, b| -b[0] * 1000 - a[0]}.each{|a, b|
  puts "#{a}\t#{b.join("\t")}"
}
puts "total length :\t#{stat.inject(0){|r, e| r+e[1][0]}}"
puts "total size   :\t#{stat.inject(0){|r, e| r+e[1][1]}}"
