--TEST--
Bug #24313: file_exists() warning on non-existant files when is open_basedir enabled
--INI--
open_basedir=/tmp
--FILE--
<?php
	var_dump(file_exists("./foobar"));
?>
--EXPECT--
bool(false)
