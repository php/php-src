--TEST--
Bug #72534 stream_socket_get_name crashes
--FILE--
<?php
$fname = dirname(__FILE__) . '/aa.tmp';
$fp0 = fopen($fname, 'w');
var_dump(stream_socket_get_name($fp0, false));
var_dump(stream_socket_get_name($fp0, true));
fclose($fp0);
?>
--CLEAN--
<?php
$fname = dirname(__FILE__) . '/aa.tmp';
unlink($fname);
?>
--EXPECT--
bool(false)
bool(false)

