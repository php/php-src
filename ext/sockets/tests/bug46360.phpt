--TEST--
Bug 46360 - TCP_NODELAY constant (sock_get_option, sock_set_option)
--CREDITS--
Florian Anderiasch
fa@php.net
--SKIPIF--
<?php
    if (version_compare(phpversion(), '5.2.7', '<')) {
        die('skip old php, not eligible');
    }
?>
--FILE--
<?php
    var_dump('TCP_NODELAY');
?>
--EXPECT--
string(11) "TCP_NODELAY"
