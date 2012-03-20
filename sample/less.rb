#! /usr/local/bin/ruby

ZCAT = "/usr/local/bin/zcat"
LESS = "/usr/local/bin/less"

FILE = ARGV.pop
OPTION = (if ARGV.length == 0; "" else ARGV.join(" "); end)

if FILE =~ /\.(Z|gz)$/
  exec(format("%s %s | %s %s", ZCAT, FILE, LESS, OPTION))
elsif FILE == nil
  exec(format("%s %s", LESS, OPTION))
else
  print(format("%s %s %s", LESS, OPTION, FILE), "\n")
  exec(format("%s %s %s", LESS, OPTION, FILE))
end
exit()
