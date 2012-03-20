max = 600000

if defined? Fiber
  gen = (1..max).each
  loop do
    gen.next
  end
else
  require 'generator'
  gen = Generator.new((0..max))
  while gen.next?
    gen.next
  end
end
