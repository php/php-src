--TEST--
default_port ini setting
--EXTENSIONS--
mysqli
--FILE--
<?php
	$orig_port = ini_get("mysqli.default_port");
    ini_set('mysqli.default_port', 65536);
	$new_port = ini_get("mysqli.default_port");
	var_dump($orig_port === $new_port);
    ini_set('mysqli.default_port', -1);
	$new_port = ini_get("mysqli.default_port");
	var_dump($orig_port === $new_port);
    ini_set('mysqli.default_port', $orig_port - 1);
	$new_port = ini_get("mysqli.default_port");
	var_dump($orig_port === $new_port);
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
