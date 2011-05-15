--TEST--
Bug #52209 (INPUT_ENV returns NULL for set variables (CLI))
--SKIPIF--
<?php if (!extension_loaded("filter") || !empty($_ENV['PWD'])) die("skip"); ?>
--INI--
variables_order=GPCSE
--FILE--
<?php
	var_dump(filter_input(INPUT_ENV, 'PWD'));
?>
--EXPECTF--	
string(%d) "%s"
