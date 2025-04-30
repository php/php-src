--TEST--
GH-18247: dba_open() memory leak on invalid path
--EXTENSIONS--
dba
--FILE--
<?php
var_dump(dba_popen('/inexistent', 'r'));
?>
--EXPECTF--

Warning: dba_popen(/inexistent): Failed to open stream: No such file or directory in %s on line %d
bool(false)
