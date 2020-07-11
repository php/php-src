--TEST--
socket_select() error conditions
--SKIPIF--
<?php
if (!extension_loaded('sockets')) die('skip socket extension not available');
?>
--FILE--
<?php
$r = $w = $e = ['no resource'];
try {
    socket_select($r, $w, $e, 1);
} catch (TypeError $ex) {
    echo $ex->getMessage(), PHP_EOL;
}
?>
--EXPECT--
socket_select(): supplied argument is not a valid Socket resource
