--TEST--
mb_ereg()
--SKIPIF--
<?php include('skipif.inc'); ?>
function_exists('mb_ereg') or die("SKIP");
--POST--
--GET--
--FILE--
<?php
	mb_regex_set_options( '' );

	$encs = array( 'EUC-JP', 'Shift_JIS', 'SJIS', 'UTF-8' );

	function test_ereg( $test_enc, $pat, $str, $in_enc = 'EUC-JP' ) {
		mb_regex_encoding( $test_enc );
		$pat = mb_convert_encoding( $pat, $test_enc, $in_enc );
		$str = mb_convert_encoding( $str, $test_enc, $in_enc );

		printf( "(%d)%s\n", mb_ereg( $pat, $str, $reg ), ( is_array( $reg )? mb_convert_encoding( implode( ' ', $reg ), $in_enc, $test_enc ) : '' ) );
	} 
	function do_tests( $enc ) {
		test_ereg( $enc, 'abc ([a-z]+) ([a-z]+) ([a-z]+)$', "abc def ghi jkl" );
		$pat = '([£á-£ú]+) ([ ¤¢-¤«]+)([¤«-¤Ê]+) ([¤ï-¤ó]+)$'; 
		test_ereg( $enc, $pat, '£á£â£ã ¤¢¤ª¤¤ ¤«¤³¤Ê ¤ï¤ñ¤ó' );
		test_ereg( $enc, $pat, '£í£ú£ø£æ£ð ¤¦¤ª¤« ¤­¤« ¤ò¤ð' );
	}

	foreach( $encs as $enc ) {
		do_tests( $enc );
	}
?>

--EXPECT--
(15)abc def ghi jkl def ghi jkl
(27)£á£â£ã ¤¢¤ª¤¤ ¤«¤³¤Ê ¤ï¤ñ¤ó £á£â£ã ¤¢¤ª¤¤ ¤« ¤³¤Ê ¤ï¤ñ¤ó
(27)£í£ú£ø£æ£ð ¤¦¤ª¤« ¤­¤« ¤ò¤ð £í£ú£ø£æ£ð ¤¦¤ª¤«  ¤­¤« ¤ò¤ð
(15)abc def ghi jkl def ghi jkl
(27)£á£â£ã ¤¢¤ª¤¤ ¤«¤³¤Ê ¤ï¤ñ¤ó £á£â£ã ¤¢¤ª¤¤ ¤« ¤³¤Ê ¤ï¤ñ¤ó
(27)£í£ú£ø£æ£ð ¤¦¤ª¤« ¤­¤« ¤ò¤ð £í£ú£ø£æ£ð ¤¦¤ª¤«  ¤­¤« ¤ò¤ð
(15)abc def ghi jkl def ghi jkl
(27)£á£â£ã ¤¢¤ª¤¤ ¤«¤³¤Ê ¤ï¤ñ¤ó £á£â£ã ¤¢¤ª¤¤ ¤« ¤³¤Ê ¤ï¤ñ¤ó
(27)£í£ú£ø£æ£ð ¤¦¤ª¤« ¤­¤« ¤ò¤ð £í£ú£ø£æ£ð ¤¦¤ª¤«  ¤­¤« ¤ò¤ð
(15)abc def ghi jkl def ghi jkl
(39)£á£â£ã ¤¢¤ª¤¤ ¤«¤³¤Ê ¤ï¤ñ¤ó £á£â£ã ¤¢¤ª¤¤ ¤« ¤³¤Ê ¤ï¤ñ¤ó
(39)£í£ú£ø£æ£ð ¤¦¤ª¤« ¤­¤« ¤ò¤ð £í£ú£ø£æ£ð ¤¦¤ª¤«  ¤­¤« ¤ò¤ð
