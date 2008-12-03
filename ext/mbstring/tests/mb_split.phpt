--TEST--
mb_split()
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
function_exists('mb_split') or die("skip mb_split() is not available in this build");
?>
--INI--
mbstring.func_overload=0
--FILE--
<?php
	mb_regex_set_options( '' );
	mb_regex_encoding( 'EUC-JP' );

	function verify_split( $spliton, $str, $count = 0 )
	{
		$result1 = mb_split( $spliton, $str, $count );
		$result2 = split( $spliton, $str, $count );
		if ( $result1 == $result2 ) {
			print "ok\n";
		} else {
			print count($result1).'-'.count($result2)."\n";
		}	
	}

	var_dump( mb_split( b" ", b"a b c d e f g" )
	          == mb_split( b"[[:space:]]", b"a\nb\tc\nd e f g" ) );

	for ( $i = 0; $i < 5; ++$i ) {
		verify_split( b" ", b"a\tb\tc\td   e\tf g", $i );
	}

	for ( $i = 1; $i < 5; ++$i ) {
		verify_split( b"\xa1\xa1+", b"\xa1\xa1\xa1\xa2\xa2\xa1\xa1\xa1\xa1\xa1\xa1\xa2\xa2\xa1\xa1\xa1", $i );
	}
?>

--EXPECT--
bool(true)
ok
ok
ok
ok
ok
ok
2-2
3-3
4-4
