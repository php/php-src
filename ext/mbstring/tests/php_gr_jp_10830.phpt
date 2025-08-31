--TEST--
php-users@php.gr.jp #10830
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_ereg') or die("skip mb_ereg() is not available in this build");
?>
--FILE--
<?php
$a="aaa\n<>";

var_dump( mb_ereg("^[^><]+$",$a) );
var_dump( !!preg_match("/^[^><]+$/",$a) );
?>
--EXPECT--
bool(false)
bool(false)
