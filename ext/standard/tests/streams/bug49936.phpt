--TEST--
Bug #49936 (crash with ftp stream in php_stream_context_get_option())
--FILE--
<?php

$dir = 'ftp://your:self@localhost/';

var_dump(opendir($dir));
var_dump(opendir($dir));

?>
--EXPECTF--
Warning: opendir(): connect() failed: Connection refused in %s on line %d

Warning: opendir(ftp://...@localhost/): failed to open dir: operation failed in %s on line %d
bool(false)

Warning: opendir(): connect() failed: Connection refused in %s on line %d

Warning: opendir(ftp://...@localhost/): failed to open dir: operation failed in %s on line %d
bool(false)
