{
	top_srcdir=$1
	top_builddir=$2
	srcdir=$3
	cmd=$4

	for (i = 5; i <= NF; i++) {
		if (match($i, "^-[A-Z]") == 0)
			break;
		cmd=cmd " " $i
	}

	dif=i-1
		
	for (; i <= NF; i++)
		filenames[i-dif]=$i
	
	no_files=NF-dif
	
	for(i = 1; i <= no_files; i++) {
		if (system("test -r " filenames[i]) != 0)
			continue
		
		target=filenames[i]
		sub(srcdir "/", "", target)
		target2=target
		sub("\.(c|cpp)$", ".lo", target);
		sub("\.(c|cpp)$", ".slo", target2);
		deplist=""

		for (e in used)
			delete used[e]
		
		cmdx=cmd " " filenames[i]
		while ((cmdx | getline) > 0) {
			sub(top_srcdir, "$(top_srcdir)", $0)
			sub(top_builddir, "$(top_builddir)", $0)
			if (match($0, "^# [0-9]* \".*\.h\"") != 0  \
					&& match($3, "^\"/") == 0          \
					&& !($3 in used)) {
				deplist=deplist " \\\n\t" substr($3,2,length($3)-2)
				used[$3] = 1;
			}
		}

		if (deplist != "")
			print target " " target2 ":" deplist "\n";
	}
} 
