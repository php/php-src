# The Computer Language Shootout
# http://shootout.alioth.debian.org
#
# contributed by jose fco. gonzalez
# modified by Sokolov Yura

seq = String.new

def frecuency( seq,length )
    n, table = seq.length - length + 1, Hash.new(0)
    f, i = nil, nil
    (0 ... length).each do |f|
        (f ... n).step(length) do |i|
            table[seq[i,length]] += 1
        end
    end
    [n,table]

end

def sort_by_freq( seq,length )
    n,table = frecuency( seq,length )
    a, b, v = nil, nil, nil
    table.sort{|a,b| b[1] <=> a[1]}.each do |v|
        puts "%s %.3f" % [v[0].upcase,((v[1]*100).to_f/n)]
    end
    puts
end

def find_seq( seq,s )
    n,table = frecuency( seq,s.length )
    puts "#{table[s].to_s}\t#{s.upcase}"
end

input = open(File.join(File.dirname($0), 'fasta.output.100000'), 'rb')

line = input.gets while line !~ /^>THREE/
line = input.gets

while (line !~ /^>/) & line do
    seq << line.chomp
    line = input.gets
end

[1,2].each {|i| sort_by_freq( seq,i ) }

%w(ggt ggta ggtatt ggtattttaatt ggtattttaatttatagt).each{|s| find_seq( seq,s) }

