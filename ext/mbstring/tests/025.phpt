--TEST--
mb_regex_set_options() 
--SKIPIF--
<?php include('skipif.inc'); ?>
function_exists('mb_regex_set_options') or die("SKIP");
--POST--
--GET--
--FILE--
<?php include('025.inc'); ?>
--EXPECT--
+ + + + +-++++
