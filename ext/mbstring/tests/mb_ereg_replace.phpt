--TEST--
mb_ereg_replace()
--SKIPIF--
<?php include('skipif.inc'); ?>
function_exists('mb_ereg_replace') or die("SKIP");
--POST--
--GET--
--FILE--
<?php
	mb_regex_set_options( '' );
	print mb_ereg_replace( ' ', '-', 'a b c d e' )."\n";
	print mb_ereg_replace( '([a-z]+)','[\\1]', 'abc def ghi' );
?>

--EXPECT--
a-b-c-d-e
[abc] [def] [ghi]

