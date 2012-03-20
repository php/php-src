# directory access
# list all files but .*/*~/*.o
dirp = Dir.open(".")
for f in dirp
  case f
  when /^\./, /~$/, /\.o/
    # do not print
  else
    print f, "\n"
  end
end
dirp.close
