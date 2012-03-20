# word occurrence listing
# usege: ruby occur.rb file..
freq = Hash.new(0)
while line = gets()
  for word in line.split(/\W+/)
    freq[word] += 1
  end
end

for word in freq.keys.sort!
  print word, " -- ", freq[word], "\n"
end
