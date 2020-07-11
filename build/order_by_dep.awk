BEGIN {
	orig_rs = RS;
	orig_fs = FS;
	RS=" ";
	mod_count = 0;
	SUBSEP=":";
}

function get_deps(module_name, module_dir,       depline, cmd)
{
	# this could probably be made *much* better
	RS=orig_rs;
	FS="[(,) \t]+"
	cmd = "grep PHP_ADD_EXTENSION_DEP " module_dir "/config*.m4"
	while (cmd | getline) {
#		printf("GOT: %s,%s,%s,%s,%s\n", $1, $2, $3, $4, $5);
		if (!length($5)) {
			$5 = 0;
		}
		mod_deps[module_name, $4] = $5;
	}
	close(cmd)
	RS=" ";
	FS=orig_fs;
}

function get_module_index(name,  i)
{
	for (i in mods) {
		if (mods[i] == name) {
			return i;
		}
	}
	return -1;
}

function do_deps(mod_idx,        module_name, mod_name_len, dep, ext, val, depidx)
{
	module_name = mods[mod_idx];
	mod_name_len = length(module_name);

	for (ext in mod_deps) {
		if (substr(ext, 0, mod_name_len+1) != module_name SUBSEP) {
			continue;
		}
		val = mod_deps[ext];
		ext = substr(ext, mod_name_len+2, length(ext)-mod_name_len);

		depidx = get_module_index(ext);
		if (depidx >= 0) {
			do_deps(depidx);
		}
	}
	printf("	phpext_%s_ptr,@NEWLINE@", module_name);
	delete mods[mod_idx];
}

function count(arr,       n, i)
{
	n = 0;
	for (i in arr)
		n++;
	return n;
}

/^[a-zA-Z0-9_;-]+/ {
	split($1, mod, ";");

	# mini hack for pedantic awk
	gsub("[^a-zA-Z0-9_-]", "", mod[1])
	# add each item to array
	mods[mod_count++] = mod[1]

	# see if it has any module deps
	get_deps(mod[1], mod[2]);
}
END {
	# order it correctly
	out_count = 0;

	while (count(mods)) {
		for (i = 0; i <= mod_count - 1; i++) {
			if (i in mods) {
				do_deps(i);
			}
		}
	}
}
