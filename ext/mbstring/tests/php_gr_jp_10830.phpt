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
--EXPECTF--
Deprecated: Function mb_ereg() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(false)
bool(false)
