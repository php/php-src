--TEST--
php-users@php.gr.jp #10830 
--SKIPIF--
<?php include('skipif.inc'); ?>
function_exists('mb_ereg') or die("SKIP");
--POST--
--GET--
--FILE--
<?php
$a="aaa\n<>";

var_dump( mb_ereg("^[^><]+$",$a) );
var_dump( ereg("^[^><]+$",$a) );
?>

--EXPECT--
bool(false)
bool(false)
