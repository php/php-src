--TEST--
mb_regex_set_options() 
--SKIPIF--
<?php include('skipif.inc'); ?>
function_exists('mb_regex_set_options') or die("SKIP");
--POST--
--GET--
--FILE--
<?php
	mb_regex_set_options( 'x' );
	print mb_ereg_replace(' -', '+', '- - - - -' );

	mb_regex_set_options( '' );
	print mb_ereg_replace(' -', '+', '- - - - -' );
?>

--EXPECT--
+ + + + +-++++
