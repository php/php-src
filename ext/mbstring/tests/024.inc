<?php
	mb_regex_set_options( '' );

	$encs = array( 'EUC-JP', 'Shift_JIS', 'SJIS', 'UTF-8' );

	function test_search( $test_enc, $str, $look_for, $opt, $in_enc = 'EUC-JP' ) {
		mb_regex_encoding( $test_enc );
		$str = mb_convert_encoding( $str, $test_enc, $in_enc );
		$look_for = mb_convert_encoding( $look_for, $test_enc, $in_enc );
		mb_ereg_search_init( $str, $look_for, $opt );
		while ( mb_ereg_search_pos() ) {
			$regs = mb_ereg_search_getregs();
			array_shift( $regs );
			printf( "(%s) (%d) %s\n", $test_enc, mb_ereg_search_getpos(), mb_convert_encoding( ( is_array( $regs ) ? implode( '-', $regs ): '' ), $in_enc, $test_enc ) );
		}
	}	
	function do_tests( $enc, $opt ) {
		test_search( $enc, "¢Ï¡¦ ¡¦¢Ï\n", ' (¡¦?¢Ï¡¦?)[[:space:]]', $opt );
		test_search( $enc, 'abcde abdeabcf anvfabc odu abcd ', '(ab[a-z]+)', $opt );
	}

	foreach( $encs as $enc ) {
		do_tests( $enc, '' );
		do_tests( $enc, 'x' );
	}
?>
