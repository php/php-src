--TEST--
mb_strtoupper() / mb_strtolower() 
--SKIPIF--
<?php include('skipif.inc'); ?>
function_exists('mb_strtolower') or die("SKIP");
--POST--
--GET--
--FILE--
<?php include('026.inc'); ?>
--EXPECT--
abcdefghijklmnopqrstuvwxyz
ABCDEFGHIJKLMNOPQRSTUVWXYZ
