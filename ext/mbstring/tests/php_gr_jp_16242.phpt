--TEST--
php-users@php.gr.jp #16242
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
?>
--INI--
mbstring.language=Japanese
mbstring.internal_encoding=UTF-8
--FILE--
<?php

var_dump(ini_get('mbstring.language'));
var_dump(ini_get('mbstring.internal_encoding'));
var_dump(mb_internal_encoding());

?>
--EXPECT--
string(8) "Japanese"
string(5) "UTF-8"
string(5) "UTF-8"
