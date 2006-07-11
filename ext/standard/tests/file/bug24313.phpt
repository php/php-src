--TEST--
Bug #24313 (file_exists() throws a warning on nonexistent files when is open_basedir enabled)
--INI--
open_basedir=/bin
--FILE--
<?php
	var_dump(file_exists("/bin/bogus_file_no_such_thing"));
?>
--EXPECT--
bool(false)
