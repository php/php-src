--TEST--
mb_strtoupper() / mb_strtolower() 
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
function_exists('mb_strtolower') and
function_exists('mb_convert_case' ) or die("skip mb_convert_case() is not available");
?>
--INI--
output_handler=
--FILE--
<?php
	mb_internal_encoding( 'ISO-8859-1' );
	print mb_strtolower( "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n" );
	print mb_strtoupper( mb_strtolower( "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n" ) );
	print mb_strtoupper( "הכן\n" );
	print mb_convert_case( "הכן\n", MB_CASE_TITLE );
?>

--EXPECT--
abcdefghijklmnopqrstuvwxyz
ABCDEFGHIJKLMNOPQRSTUVWXYZ
ִֻֿ
ִכן
