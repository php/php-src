--TEST--
Bug #66486 strtotime only with whitespaces should return false
--CREDITS--
Gabriel Caruso (carusogabriel@php.net)
--FILE--
<?php
var_dump(
    strtotime(''),
    strtotime(' '),
    strtotime('  ')
);
?>
--EXPECTF--
bool(false)
bool(false)
bool(false)
