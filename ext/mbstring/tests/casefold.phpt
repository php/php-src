--TEST--
mb_strtoupper() / mb_strtolower() 
--SKIPIF--
<?php include('skipif.inc'); ?>
function_exists('mb_strtolower') or die("SKIP");
--POST--
--GET--
--FILE--
<?php
	mb_internal_encoding( 'ISO-8859-1' );
	print mb_strtolower( "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n" );
	print mb_strtoupper( mb_strtolower( "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n" ) );
	print mb_strtoupper( "äëï\n" );
?>

--EXPECT--
abcdefghijklmnopqrstuvwxyz
ABCDEFGHIJKLMNOPQRSTUVWXYZ
ÄËÏ

