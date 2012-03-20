# word occurrence listing
# usege: ruby occur2.rb file..
freq = {}
ARGF.each_line do |line|
  for word in line.split(/\W+/)
    freq[word] ||= 0
    freq[word] += 1
  end
end

for word in freq.keys.sort
  printf("%s -- %d\n", word, freq[word])
end
