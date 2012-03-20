#!/usr/bin/ruby
# -*- mode: ruby -*-
# $Id: matrix-ruby.code,v 1.4 2004/11/13 07:42:14 bfulgham Exp $
# http://www.bagley.org/~doug/shootout/

n = 60 #Integer(ARGV.shift || 1)

size = 40

def mkmatrix(rows, cols)
    count = 1
    mx = Array.new(rows)
    (0 .. (rows - 1)).each do |bi|
        row = Array.new(cols, 0)
        (0 .. (cols - 1)).each do |j|
            row[j] = count
            count += 1
        end
        mx[bi] = row
    end
    mx
end

def mmult(rows, cols, m1, m2)
    m3 = Array.new(rows)
    (0 .. (rows - 1)).each do |bi|
        row = Array.new(cols, 0)
        (0 .. (cols - 1)).each do |j|
            val = 0
            (0 .. (cols - 1)).each do |k|
                val += m1.at(bi).at(k) * m2.at(k).at(j)
            end
            row[j] = val
        end
        m3[bi] = row
    end
    m3
end

m1 = mkmatrix(size, size)
m2 = mkmatrix(size, size)
mm = Array.new
n.times do
    mm = mmult(size, size, m1, m2)
end
# puts "#{mm[0][0]} #{mm[2][3]} #{mm[3][2]} #{mm[4][4]}"


