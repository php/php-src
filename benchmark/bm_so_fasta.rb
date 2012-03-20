# The Computer Language Shootout
# http://shootout.alioth.debian.org/
# Contributed by Sokolov Yura

$last = 42.0
def gen_random (max,im=139968,ia=3877,ic=29573)
    (max * ($last = ($last * ia + ic) % im)) / im
end

alu =
   "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGG"+
   "GAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGA"+
   "CCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAAT"+
   "ACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCA"+
   "GCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGG"+
   "AGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCC"+
   "AGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA"

iub = [
    ["a", 0.27],
    ["c", 0.12],
    ["g", 0.12],
    ["t", 0.27],

    ["B", 0.02],
    ["D", 0.02],
    ["H", 0.02],
    ["K", 0.02],
    ["M", 0.02],
    ["N", 0.02],
    ["R", 0.02],
    ["S", 0.02],
    ["V", 0.02],
    ["W", 0.02],
    ["Y", 0.02],
]
homosapiens = [
    ["a", 0.3029549426680],
    ["c", 0.1979883004921],
    ["g", 0.1975473066391],
    ["t", 0.3015094502008],
]

def make_repeat_fasta(id, desc, src, n)
    puts ">#{id} #{desc}"
    v = nil
    width = 60
    l = src.length
    s = src * ((n / l) + 1)
    s.slice!(n, l)
    puts(s.scan(/.{1,#{width}}/).join("\n"))
end

def make_random_fasta(id, desc, table, n)
    puts ">#{id} #{desc}"
    rand, v = nil,nil
    width = 60
    chunk = 1 * width
    prob = 0.0
    table.each{|v| v[1]= (prob += v[1])}
    for i in 1..(n/width)
        puts((1..width).collect{
            rand = gen_random(1.0)
            table.find{|v| v[1]>rand}[0]
        }.join)
    end
    if n%width != 0
        puts((1..(n%width)).collect{
            rand = gen_random(1.0)
            table.find{|v| v[1]>rand}[0]
        }.join)
    end
end


n = (ARGV[0] or 250_000).to_i

make_repeat_fasta('ONE', 'Homo sapiens alu', alu, n*2)
make_random_fasta('TWO', 'IUB ambiguity codes', iub, n*3)
make_random_fasta('THREE', 'Homo sapiens frequency', homosapiens, n*5)

