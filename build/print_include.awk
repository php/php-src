/phpext_/ {
	if (old_filename != FILENAME) {
		printf "#include \"" FILENAME "\"\\\\n"
		old_filename = FILENAME
	}
}
