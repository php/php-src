# word occurrence listing
# usege: ruby freq.rb file..
freq = Hash.new(0)
while line = gets()
  line.scan(/\w+/) do |word|
    freq[word] += 1
  end
end

for word in freq.keys.sort!
  print word, " -- ", freq[word], "\n"
end
