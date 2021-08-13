--TEST--
socket_select() error conditions
--EXTENSIONS--
sockets
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
socket_select(): Argument #1 ($read) must only have elements of type Socket, string given
