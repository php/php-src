#! ./miniruby
while gets
  if ~/enum node_type \{/..~/^\};/
    ~/(NODE_.+),/ and puts("      case #{$1}:\n\treturn \"#{$1}\";")
  end
end
