--TEST--
ext/sockets - socket_bind - test with empty parameters
--CREDITS--
Florian Anderiasch
fa@php.net
--SKIPIF--
<?php
    if (!extension_loaded('sockets')) {
        die('skip - sockets extension not available.');
    }
?>
--FILE--
<?php
    $rand = rand(1,999);
    $s_c = socket_create_listen(31330+$rand);
    $s_w = socket_bind();
    var_dump($s_w);
    $s_w = socket_bind($s_c);
    var_dump($s_w);
    socket_close($s_c);

?>
--EXPECTF--
Warning: socket_bind() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: socket_bind() expects at least 2 parameters, 1 given in %s on line %d
NULL
