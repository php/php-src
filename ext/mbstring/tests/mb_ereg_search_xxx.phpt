--TEST--
mb_ereg_search() stuff
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
function_exists('mb_ereg_search') or die("skip\n");
?>
--INI--
output_handler=
--FILE--
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
--EXPECT--
(EUC-JP) (10) ¡¦¢Ï
(EUC-JP) (5) abcde
(EUC-JP) (14) abdeabcf
(EUC-JP) (22) abc
(EUC-JP) (31) abcd
(EUC-JP) (5) ¢Ï¡¦
(EUC-JP) (10) ¡¦¢Ï
(EUC-JP) (5) abcde
(EUC-JP) (14) abdeabcf
(EUC-JP) (22) abc
(EUC-JP) (31) abcd
(Shift_JIS) (10) ¡¦¢Ï
(Shift_JIS) (5) abcde
(Shift_JIS) (14) abdeabcf
(Shift_JIS) (22) abc
(Shift_JIS) (31) abcd
(Shift_JIS) (5) ¢Ï¡¦
(Shift_JIS) (10) ¡¦¢Ï
(Shift_JIS) (5) abcde
(Shift_JIS) (14) abdeabcf
(Shift_JIS) (22) abc
(Shift_JIS) (31) abcd
(SJIS) (10) ¡¦¢Ï
(SJIS) (5) abcde
(SJIS) (14) abdeabcf
(SJIS) (22) abc
(SJIS) (31) abcd
(SJIS) (5) ¢Ï¡¦
(SJIS) (10) ¡¦¢Ï
(SJIS) (5) abcde
(SJIS) (14) abdeabcf
(SJIS) (22) abc
(SJIS) (31) abcd
(UTF-8) (14) ¡¦¢Ï
(UTF-8) (5) abcde
(UTF-8) (14) abdeabcf
(UTF-8) (22) abc
(UTF-8) (31) abcd
(UTF-8) (7) ¢Ï¡¦
(UTF-8) (14) ¡¦¢Ï
(UTF-8) (5) abcde
(UTF-8) (14) abdeabcf
(UTF-8) (22) abc
(UTF-8) (31) abcd
