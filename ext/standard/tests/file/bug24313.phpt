--TEST--
Bug #24313 (file_exists() throws a warning on nonexistent files when is open_basedir enabled)
--INI--
open_basedir=/dev
--FILE--
<?php
	var_dump(file_exists("/dev/bogus_file_no_such_thing"));
?>
--EXPECT--
bool(false)
