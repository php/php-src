--TEST--
Bug #24313: file_exists() warning on non-existant files when is open_basedir enabled
--INI--
open_basedir=/tmp
--FILE--
<?php
	var_dump(file_exists("/tmp/bogus_file_no_such_thing"));
?>
--EXPECT--
bool(false)
