--TEST--
mb_strtoupper() / mb_strtolower() 
--SKIPIF--
<?php include('skipif.inc'); ?>
function_exists('mb_strtolower') or die("SKIP");
--POST--
--GET--
--FILE--
<?php
	print mb_strtolower( "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n" );
	print mb_strtoupper( mb_strtolower( "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n" ) );
?>

--EXPECT--
abcdefghijklmnopqrstuvwxyz
ABCDEFGHIJKLMNOPQRSTUVWXYZ
