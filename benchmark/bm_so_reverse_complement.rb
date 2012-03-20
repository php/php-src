#!/usr/bin/ruby
# The Great Computer Language Shootout
# http://shootout.alioth.debian.org/
#
# Contributed by Peter Bjarke Olsen
# Modified by Doug King

seq=Array.new

def revcomp(seq)
  seq.reverse!.tr!('wsatugcyrkmbdhvnATUGCYRKMBDHVN','WSTAACGRYMKVHDBNTAACGRYMKVHDBN')
  stringlen=seq.length
  0.step(stringlen-1,60) {|x| print seq.slice(x,60) , "\n"}
end

input = open(File.join(File.dirname($0), 'fasta.output.2500000'), 'rb')

while input.gets
  if $_ =~ />/
    if seq.length != 0
      revcomp(seq.join)
      seq=Array.new
    end
    puts $_
  else
    $_.sub(/\n/,'')
    seq.push $_
  end
end
revcomp(seq.join)
