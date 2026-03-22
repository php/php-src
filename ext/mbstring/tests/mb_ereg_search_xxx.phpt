--TEST--
mb_ereg_search() stuff
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
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
--EXPECTF--
Deprecated: Function mb_regex_set_options() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(EUC-JP) (10) ¡¦¢Ï

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(EUC-JP) (5) abcde

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(EUC-JP) (14) abdeabcf

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(EUC-JP) (22) abc

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(EUC-JP) (31) abcd

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(EUC-JP) (5) ¢Ï¡¦

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(EUC-JP) (10) ¡¦¢Ï

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(EUC-JP) (5) abcde

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(EUC-JP) (14) abdeabcf

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(EUC-JP) (22) abc

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(EUC-JP) (31) abcd

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(Shift_JIS) (10) ¡¦¢Ï

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(Shift_JIS) (5) abcde

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(Shift_JIS) (14) abdeabcf

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(Shift_JIS) (22) abc

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(Shift_JIS) (31) abcd

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(Shift_JIS) (5) ¢Ï¡¦

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(Shift_JIS) (10) ¡¦¢Ï

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(Shift_JIS) (5) abcde

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(Shift_JIS) (14) abdeabcf

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(Shift_JIS) (22) abc

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(Shift_JIS) (31) abcd

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(SJIS) (10) ¡¦¢Ï

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(SJIS) (5) abcde

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(SJIS) (14) abdeabcf

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(SJIS) (22) abc

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(SJIS) (31) abcd

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(SJIS) (5) ¢Ï¡¦

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(SJIS) (10) ¡¦¢Ï

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(SJIS) (5) abcde

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(SJIS) (14) abdeabcf

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(SJIS) (22) abc

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(SJIS) (31) abcd

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(UTF-8) (14) ¡¦¢Ï

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(UTF-8) (5) abcde

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(UTF-8) (14) abdeabcf

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(UTF-8) (22) abc

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(UTF-8) (31) abcd

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(UTF-8) (7) ¢Ï¡¦

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(UTF-8) (14) ¡¦¢Ï

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(UTF-8) (5) abcde

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(UTF-8) (14) abdeabcf

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(UTF-8) (22) abc

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
(UTF-8) (31) abcd

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_search_xxx.php on line %d
