--TEST--
Bug #72075 (Referencing socket resources breaks stream_select)
--FILE--
<?php
$r = [stream_socket_server("tcp://127.0.0.1:0", $errno, $errStr)];
$w = NULL;
$e = NULL;

// Without this line, all is well:
$dummy =& $r[0];

print stream_select($r, $w, $e, 0.5);
?>
--EXPECT--
0
