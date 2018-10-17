--TEST--
mb_ereg_replace()
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
function_exists('mb_ereg_replace') or die("skip mb_ereg_replace() is not available in this build");
?>
--FILE--
<?php
	mb_regex_set_options( '' );
	print mb_ereg_replace( ' ', '-', 'a b c d e' )."\n";
	print mb_ereg_replace( '([a-z]+)','[\\1]', 'abc def ghi' );
?>
--EXPECT--
a-b-c-d-e
[abc] [def] [ghi]
