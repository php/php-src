--TEST--
mb_regex_set_options() 
--SKIPIF--
<?php
extension_loaded('mbstring') or die("skip mbstring not available\n");
function_exists('mb_regex_set_options') or die("skip\n");
?>
--FILE--
<?php
	mb_regex_set_options( 'x' );
	print mb_ereg_replace(' -', '+', '- - - - -' );

	mb_regex_set_options( '' );
	print mb_ereg_replace(' -', '+', '- - - - -' );
?>

--EXPECT--
+ + + + +-++++
