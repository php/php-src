#!/usr/local/bin/ruby
# This program is contributed by Shin Nishiyama


# modified by K.Sasada

NP = 5
ROW = 8 + NP
COL = 8

$p = []
$b = []
$no = 0

def piece(n, a, nb)
  nb.each{|x|
    a[n] = x
    if n == NP-1
      $p << [a.sort]
    else
      nbc=nb.dup
      [-ROW, -1, 1, ROW].each{|d|
        if x+d > 0 and not a.include?(x+d) and not nbc.include?(x+d)
          nbc << x+d
        end
      }
      nbc.delete x
      piece(n+1,a[0..n],nbc)
    end
  }
end

def kikaku(a)
  a.collect {|x| x - a[0]}
end
def ud(a)
  kikaku(a.collect {|x| ((x+NP)%ROW)-ROW*((x+NP)/ROW) }.sort)
end
def rl(a)
  kikaku(a.collect {|x| ROW*((x+NP)/ROW)+ROW-((x+NP)%ROW)}.sort)
end
def xy(a)
  kikaku(a.collect {|x| ROW*((x+NP)%ROW) + (x+NP)/ROW }.sort)
end

def mkpieces
  piece(0,[],[0])
  $p.each do |a|
    a0 = a[0]
    a[1] = ud(a0)
    a[2] = rl(a0)
    a[3] = ud(rl(a0))
    a[4] = xy(a0)
    a[5] = ud(xy(a0))
    a[6] = rl(xy(a0))
    a[7] = ud(rl(xy(a0)))
    a.sort!
    a.uniq!
  end
  $p.uniq!.sort! {|x,y| x[0] <=> y[0] }
end

def mkboard
  (0...ROW*COL).each{|i|
    if i % ROW >= ROW-NP
      $b[i] = -2
    else
      $b[i] = -1
    end
    $b[3*ROW+3]=$b[3*ROW+4]=$b[4*ROW+3]=$b[4*ROW+4]=-2
  }
end

def pboard
  return # skip print
  print "No. #$no\n"
  (0...COL).each{|i|
    print "|"
    (0...ROW-NP).each{|j|
      x = $b[i*ROW+j]
      if x < 0
        print "..|"
      else
        printf "%2d|",x+1
      end
    }
    print "\n"
  }
  print "\n"
end

$pnum=[]
def setpiece(a,pos)
  if a.length == $p.length then
    $no += 1
    pboard
    return
  end
  while $b[pos] != -1
    pos += 1
  end
  ($pnum - a).each do |i|
    $p[i].each do |x|
      f = 0
      x.each{|s|
        if $b[pos+s] != -1
          f=1
          break
        end
      }
      if f == 0 then
        x.each{|s|
          $b[pos+s] = i
        }
        a << i
        setpiece(a.dup, pos)
        a.pop
        x.each{|s|
          $b[pos+s] = -1
        }
      end
    end
  end
end

mkpieces
mkboard
$p[4] = [$p[4][0]]
$pnum = (0...$p.length).to_a
setpiece([],0)


__END__

# original

NP = 5
ROW = 8 + NP
COL = 8

$p = []
$b = []
$no = 0

def piece(n,a,nb)
  for x in nb
    a[n] = x
    if n == NP-1
      $p << [a.sort]
    else
      nbc=nb.dup
      for d in [-ROW, -1, 1, ROW]
        if x+d > 0 and not a.include?(x+d) and not nbc.include?(x+d)
          nbc << x+d
        end
      end
      nbc.delete x
      piece(n+1,a[0..n],nbc)
    end
  end
end

def kikaku(a)
  a.collect {|x| x - a[0]}
end
def ud(a)
  kikaku(a.collect {|x| ((x+NP)%ROW)-ROW*((x+NP)/ROW) }.sort)
end
def rl(a)
  kikaku(a.collect {|x| ROW*((x+NP)/ROW)+ROW-((x+NP)%ROW)}.sort)
end
def xy(a)
  kikaku(a.collect {|x| ROW*((x+NP)%ROW) + (x+NP)/ROW }.sort)
end

def mkpieces
  piece(0,[],[0])
  $p.each do |a|
    a0 = a[0]
    a[1] = ud(a0)
    a[2] = rl(a0)
    a[3] = ud(rl(a0))
    a[4] = xy(a0)
    a[5] = ud(xy(a0))
    a[6] = rl(xy(a0))
    a[7] = ud(rl(xy(a0)))
    a.sort!
    a.uniq!
  end
  $p.uniq!.sort! {|x,y| x[0] <=> y[0] }
end

def mkboard
  for i in 0...ROW*COL
    if i % ROW >= ROW-NP
      $b[i] = -2
    else
      $b[i] = -1
    end
    $b[3*ROW+3]=$b[3*ROW+4]=$b[4*ROW+3]=$b[4*ROW+4]=-2
  end
end

def pboard
  print "No. #$no\n"
  for i in 0...COL
    print "|"
    for j in 0...ROW-NP
      x = $b[i*ROW+j]
      if x < 0
        print "..|"
      else
        printf "%2d|",x+1
      end
    end
    print "\n"
  end
  print "\n"
end

$pnum=[]
def setpiece(a,pos)
  if a.length == $p.length then
    $no += 1
    pboard
    return
  end
  while $b[pos] != -1
    pos += 1
  end
  ($pnum - a).each do |i|
    $p[i].each do |x|
      f = 0
      for s in x do
        if $b[pos+s] != -1
          f=1
          break
        end
      end
      if f == 0 then
        for s in x do
          $b[pos+s] = i
        end
        a << i
        setpiece(a.dup, pos)
        a.pop
        for s in x do
          $b[pos+s] = -1
        end
      end
    end
  end
end

mkpieces
mkboard
$p[4] = [$p[4][0]]
$pnum = (0...$p.length).to_a
setpiece([],0)
