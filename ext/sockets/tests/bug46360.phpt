--TEST--
Bug 46360 - TCP_NODELAY constant (sock_get_option, sock_set_option)
--CREDITS--
Florian Anderiasch
fa@php.net
--FILE--
<?php
    var_dump('TCP_NODELAY');
?>
--EXPECT--
string(11) "TCP_NODELAY"
