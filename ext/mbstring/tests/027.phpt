--TEST--
php-users@php.gr.jp #10830 
--SKIPIF--
<?php include('skipif.inc'); ?>
function_exists('mb_ereg') or die("SKIP");
--POST--
--GET--
--FILE--
<?php include('027.inc'); ?>
--EXPECT--
bool(false)
bool(false)
