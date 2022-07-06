--TEST--
grapheme()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
<?php if (version_compare(INTL_ICU_VERSION, '65.0') < 0) die('skip for ICU >= 65.0'); ?>
--FILE--
<?php

/*
 * Test grapheme functions (procedural only)
 */

function ut_main()
{
    $res_str = '';

    $char_a_diaeresis = "\xC3\xA4";	// 'LATIN SMALL LETTER A WITH DIAERESIS' (U+00E4)
    $char_a_ring = "\xC3\xA5";		// 'LATIN SMALL LETTER A WITH RING ABOVE' (U+00E5)
    $char_o_diaeresis = "\xC3\xB6";    // 'LATIN SMALL LETTER O WITH DIAERESIS' (U+00F6)
    $char_O_diaeresis = "\xC3\x96";    // 'LATIN CAPITAL LETTER O WITH DIAERESIS' (U+00D6)

    $char_angstrom_sign = "\xE2\x84\xAB"; // 'ANGSTROM SIGN' (U+212B)
    $char_A_ring = "\xC3\x85";	// 'LATIN CAPITAL LETTER A WITH RING ABOVE' (U+00C5)

    $char_ohm_sign = "\xE2\x84\xA6";	// 'OHM SIGN' (U+2126)
    $char_omega = "\xCE\xA9";  // 'GREEK CAPITAL LETTER OMEGA' (U+03A9)

    $char_combining_ring_above = "\xCC\x8A";  // 'COMBINING RING ABOVE' (U+030A)

    $char_fi_ligature = "\xEF\xAC\x81";  // 'LATIN SMALL LIGATURE FI' (U+FB01)

    $char_long_s_dot = "\xE1\xBA\x9B";	// 'LATIN SMALL LETTER LONG S WITH DOT ABOVE' (U+1E9B)

    // the word 'hindi' using Devanagari characters:
    $hindi = "\xe0\xa4\xb9\xe0\xa4\xbf\xe0\xa4\xa8\xe0\xa5\x8d\xe0\xa4\xa6\xe0\xa5\x80";

    $char_a_ring_nfd = "a\xCC\x8A";
    $char_A_ring_nfd = "A\xCC\x8A";
    $char_o_diaeresis_nfd = "o\xCC\x88";
    $char_O_diaeresis_nfd = "O\xCC\x88";
    $char_diaeresis = "\xCC\x88";

    //=====================================================================================
    $res_str .= "\n" . 'function grapheme_strlen($string) {}' . "\n\n";


    $res_str .= "\"hindi\" in devanagari strlen " . grapheme_strlen($hindi) . "\n";
    $res_str .= "\"ab\" + \"hindi\" + \"cde\" strlen " . grapheme_strlen('ab' . $hindi . 'cde') . "\n";
    $res_str .= "\"\" strlen " . grapheme_strlen("") . "\n";
    $res_str .= "char_a_ring_nfd strlen " . grapheme_strlen($char_a_ring_nfd) . "\n";
    $res_str .= "char_a_ring_nfd + \"bc\" strlen " . grapheme_strlen($char_a_ring_nfd . 'bc') . "\n";
    $res_str .= "\"abc\" strlen " . grapheme_strlen('abc') . "\n";


    //=====================================================================================
    $res_str .= "\n" . 'function grapheme_strpos($haystack, $needle, $offset = 0) {}' . "\n\n";

    $tests = array(
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "o", "o", 5 ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd, "o", "false" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd, $char_o_diaeresis_nfd, 4 ),
        array( $char_o_diaeresis_nfd . "a" . $char_a_ring_nfd . "bc", $char_a_ring_nfd, 2 ),
        array( "a" . $char_a_ring_nfd . "bc", $char_a_ring_nfd, 1 ),
        array( "abc", $char_a_ring_nfd, "false" ),
        array( $char_a_ring_nfd . "bc", "a", "false" ),
        array( "abc", "d", "false" ),
        array( "abc", "c", 2 ),
        array( "abc", "b", 1 ),
        array( "abc", "a", 0 ),
        array( "abc", "a", 0, 0 ),
        array( "abc", "a", 1, "false" ),
        array( "abc", "a", -1, "false" ),
        array( "ababc", "a", 1, 2 ),
        array( "ao" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "o", "o", 2, 6 ),
        array( "ao" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "o", "o", -1, 6 ),
        array( "ao" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "o", "o", -5, 6 ),
        array( $char_o_diaeresis_nfd . $char_a_ring_nfd . "a" . $char_a_ring_nfd . "bc", $char_a_ring_nfd, 2, 3 ),
        array( $char_o_diaeresis_nfd . $char_a_ring_nfd . "a" . $char_a_ring_nfd . "bc", $char_a_ring_nfd, -4, 3 ),

        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "opq", "op", 5 ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "opq", "opq", 5 ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd, "abc", "false" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "bc" . $char_o_diaeresis_nfd, $char_o_diaeresis_nfd . "bc" . $char_o_diaeresis_nfd, 4 ),
        array( $char_o_diaeresis_nfd . "a" . $char_a_ring_nfd . "bc", $char_a_ring_nfd . "bc", 2 ),
        array( "a" . $char_a_ring_nfd . "bc", $char_a_ring_nfd . "bc", 1 ),
        array( "abc", $char_a_ring_nfd . "bc", "false" ),
        array( $char_a_ring_nfd . "bc", "abcdefg", "false" ),
        array( "abc", "defghijklmnopq", "false" ),
        array( "abc", "ab", 0 ),
        array( "abc", "bc", 1 ),
        array( "abc", "abc", 0 ),
        array( "abc", "abcd", "false" ),
        array( "abc", "ab", 0, 0 ),
        array( "abc", "abc", 0, 0 ),
        array( "abc", "abc", 1, "false" ),
        array( "ababc", "ab", 1, 2 ),
        array( "ababc", "abc", 1, 2 ),
        array( "ao" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "o" . $char_a_ring_nfd . "bc", "o" . $char_a_ring_nfd . "bc", 2, 6 ),
        array( "ao" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "o" . $char_a_ring_nfd . "bc", "o" . $char_a_ring_nfd . "bc", -8, 6 ),
        array( $char_o_diaeresis_nfd . $char_a_ring_nfd . "a" . $char_a_ring_nfd . "bc" . $char_a_ring_nfd . "def", $char_a_ring_nfd . "bc" . $char_a_ring_nfd, 2, 3 ),
    );

    foreach( $tests as $test ) {
        $arg1 = urlencode($test[1]);
        $arg0 = urlencode($test[0]);
        $res_str .= "find \"$arg1\" in \"$arg0\" - grapheme_strpos";
        if ( 3 == count( $test ) ) {
            $result = grapheme_strpos($test[0], $test[1]);
        }
        else {
            $res_str .= " from $test[2]";
            $result = grapheme_strpos($test[0], $test[1], $test[2]);
        }
        $res_str .= " = ";
        if ( $result === false ) {
            $res_str .= 'false';
        }
        else {
            $res_str .= $result;
        }
        $res_str .= " == " . $test[count($test)-1] . check_result($result, $test[count($test)-1]) . "\n";
    }

    //=====================================================================================
    $res_str .= "\n" . 'function grapheme_stripos($haystack, $needle, $offset = 0) {}' . "\n\n";

    $tests = array(
        array( "ao" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "O", "o", 2, 6 ),
        array( "ao" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Oo", "o", -6, 6 ),
        array( $char_o_diaeresis_nfd . $char_a_ring_nfd . "a" . $char_A_ring_nfd . "bc", $char_a_ring_nfd, 2, 3 ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "O", "o", 5 ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd, "O", "false" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_O_diaeresis_nfd, $char_o_diaeresis_nfd, 4 ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_O_diaeresis_nfd, $char_o_diaeresis_nfd, -1, 4 ),
        array( $char_o_diaeresis_nfd . "a" . $char_a_ring_nfd . "bc", $char_A_ring_nfd, 2 ),
        array( "a" . $char_A_ring_nfd . "bc", $char_a_ring_nfd, 1 ),
        array( "Abc", $char_a_ring_nfd, "false" ),
        array( $char_a_ring_nfd . "bc", "A", "false" ),
        array( "abc", "D", "false" ),
        array( "abC", "c", 2 ),
        array( "abc", "B", 1 ),
        array( "Abc", "a", 0 ),
        array( "abc", "A", 0, 0 ),
        array( "Abc", "a", 1, "false" ),
        array( "ababc", "A", 1, 2 ),

        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", "oP", 5 ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", "opQ", 5 ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd, "abc", "false" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "bC" . $char_o_diaeresis_nfd, $char_O_diaeresis_nfd . "bc" . $char_o_diaeresis_nfd, 4 ),
        array( $char_o_diaeresis_nfd . "a" . $char_a_ring_nfd . "Bc", $char_A_ring_nfd . "bc", 2 ),
        array( "a" . $char_a_ring_nfd . "BC", $char_a_ring_nfd . "bc", 1 ),
        array( "abc", $char_a_ring_nfd . "BC", "false" ),
        array( $char_a_ring_nfd . "BC", "aBCdefg", "false" ),
        array( "aBC", "Defghijklmnopq", "false" ),
        array( "abC", "Ab", 0 ),
        array( "aBC", "bc", 1 ),
        array( "abC", "Abc", 0 ),
        array( "abC", "aBcd", "false" ),
        array( "ABc", "ab", 0, 0 ),
        array( "aBc", "abC", 0, 0 ),
        array( "abc", "aBc", 1, "false" ),
        array( "ABabc", "AB", 1, 2 ),
        array( "ABabc", "AB", -4, 2 ),
        array( "abaBc", "aBc", 1, 2 ),
        array( "ao" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "o" . $char_A_ring_nfd . "bC", "O" . $char_a_ring_nfd . "bC", 2, 6 ),
        array( $char_o_diaeresis_nfd . $char_a_ring_nfd . "a" . $char_A_ring_nfd . "bC" . $char_a_ring_nfd . "def", $char_a_ring_nfd . "Bc" . $char_a_ring_nfd, 2, 3 ),
    );

    foreach( $tests as $test ) {
        $arg1 = urlencode($test[1]);
        $arg0 = urlencode($test[0]);
        $res_str .= "find \"$arg1\" in \"$arg0\" - grapheme_stripos";
        if ( 3 == count( $test ) ) {
            $result = grapheme_stripos($test[0], $test[1]);
        }
        else {
            $res_str .= " from $test[2]";
            $result = grapheme_stripos($test[0], $test[1], $test[2]);
        }
        $res_str .= " = ";
        if ( $result === false ) {
            $res_str .= 'false';
        }
        else {
            $res_str .= $result;
        }
        $res_str .= " == " . $test[count($test)-1] . check_result($result, $test[count($test)-1]) . "\n";
    }


    //=====================================================================================
    $res_str .= "\n" . 'function grapheme_strrpos($haystack, $needle, $offset = 0) {}' . "\n\n";


    $tests = array(
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "o", "o", 5 ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd, "o", "false" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd, $char_o_diaeresis_nfd, 4 ),
        array( $char_o_diaeresis_nfd . "a" . $char_a_ring_nfd . "bc", $char_a_ring_nfd, 2 ),
        array( "a" . $char_a_ring_nfd . "bc", $char_a_ring_nfd, 1 ),
        array( "abc", $char_a_ring_nfd, "false" ),
        array( $char_a_ring_nfd . "bc", "a", "false" ),
        array( "abc", "d", "false" ),
        array( "abc", "c", 2 ),
        array( "abc", "b", 1 ),
        array( "abc", "a", 0 ),
        array( "abc", "a", 0, 0 ),
        array( "abc", "a", 1, "false" ),
        array( "ababc", "a", 1, 2 ),
        array( "ao" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "o", "o", 2, 6 ),
        array( $char_o_diaeresis_nfd . $char_a_ring_nfd . "a" . $char_a_ring_nfd . "bc", $char_a_ring_nfd, 2, 3 ),

        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "opq", "op", 5 ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "opq", "opq", 5 ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd, "abc", "false" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "bc" . $char_o_diaeresis_nfd, $char_o_diaeresis_nfd . "bc" . $char_o_diaeresis_nfd, 4 ),
        array( $char_o_diaeresis_nfd . "a" . $char_a_ring_nfd . "bc", $char_a_ring_nfd . "bc", 2 ),
        array( "a" . $char_a_ring_nfd . "bc", $char_a_ring_nfd . "bc", 1 ),
        array( "abc", $char_a_ring_nfd . "bc", "false" ),
        array( $char_a_ring_nfd . "bc", "abcdefg", "false" ),
        array( "abc", "defghijklmnopq", "false" ),
        array( "abc", "ab", 0 ),
        array( "abc", "bc", 1 ),
        array( "abc", "abc", 0 ),
        array( "abc", "abcd", "false" ),
        array( "abc", "ab", 0, 0 ),
        array( "abc", "abc", 0, 0 ),
        array( "abc", "abc", 1, "false" ),
        array( "ababc", "ab", 1, 2 ),
        array( "ababc", "abc", 1, 2 ),
        array( "ao" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "o" . $char_a_ring_nfd . "bc", "o" . $char_a_ring_nfd . "bc", 2, 6 ),
        array( $char_o_diaeresis_nfd . $char_a_ring_nfd . "a" . $char_a_ring_nfd . "bc" . $char_a_ring_nfd . "def", $char_a_ring_nfd . "bc" . $char_a_ring_nfd, 2, 3 ),
    );

    foreach( $tests as $test ) {
        $arg1 = urlencode($test[1]);
        $arg0 = urlencode($test[0]);
        $res_str .= "find \"$arg1\" in \"$arg0\" - grapheme_strrpos";
        if ( 3 == count( $test ) ) {
            $result = grapheme_strrpos($test[0], $test[1]);
        }
        else {
            $res_str .= " from $test[2]";
            $result = grapheme_strrpos($test[0], $test[1], $test[2]);
        }
        $res_str .= " = ";
        if ( $result === false ) {
            $res_str .= 'false';
        }
        else {
            $res_str .= $result;
        }
        $res_str .= " == " . $test[count($test)-1] .  check_result($result, $test[count($test)-1]) . "\n";
    }


    //=====================================================================================
    $res_str .= "\n" . 'function grapheme_strripos($haystack, $needle, $offset = 0) {}' . "\n\n";

    $tests = array(
        array( "ao" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "O", "o", 2, 6 ),
        array( $char_o_diaeresis_nfd . $char_a_ring_nfd . "a" . $char_A_ring_nfd . "bc", $char_a_ring_nfd, 2, 3 ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "O", "o", 5 ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd, "O", "false" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_O_diaeresis_nfd, $char_o_diaeresis_nfd, 4 ),
        array( $char_o_diaeresis_nfd . "a" . $char_a_ring_nfd . "bc", $char_A_ring_nfd, 2 ),
        array( "a" . $char_A_ring_nfd . "bc", $char_a_ring_nfd, 1 ),
        array( "Abc", $char_a_ring_nfd, "false" ),
        array( $char_a_ring_nfd . "bc", "A", "false" ),
        array( "abc", "D", "false" ),
        array( "abC", "c", 2 ),
        array( "abc", "B", 1 ),
        array( "Abc", "a", 0 ),
        array( "abc", "A", 0, 0 ),
        array( "Abc", "a", 1, "false" ),
        array( "ababc", "A", 1, 2 ),

        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", "oP", 5 ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", "opQ", 5 ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd, "abc", "false" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "bC" . $char_o_diaeresis_nfd, $char_O_diaeresis_nfd . "bc" . $char_o_diaeresis_nfd, 4 ),
        array( $char_o_diaeresis_nfd . "a" . $char_a_ring_nfd . "Bc", $char_A_ring_nfd . "bc", 2 ),
        array( "a" . $char_a_ring_nfd . "BC", $char_a_ring_nfd . "bc", 1 ),
        array( "abc", $char_a_ring_nfd . "BC", "false" ),
        array( $char_a_ring_nfd . "BC", "aBCdefg", "false" ),
        array( "aBC", "Defghijklmnopq", "false" ),
        array( "abC", "Ab", 0 ),
        array( "aBC", "bc", 1 ),
        array( "abC", "Abc", 0 ),
        array( "abC", "aBcd", "false" ),
        array( "ABc", "ab", 0, 0 ),
        array( "aBc", "abC", 0, 0 ),
        array( "abc", "aBc", 1, "false" ),
        array( "ABabc", "AB", 1, 2 ),
        array( "abaBc", "aBc", 1, 2 ),
        array( "ao" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "o" . $char_A_ring_nfd . "bC", "O" . $char_a_ring_nfd . "bC", 2, 6 ),
        array( $char_o_diaeresis_nfd . $char_a_ring_nfd . "a" . $char_A_ring_nfd . "bC" . $char_a_ring_nfd . "def", $char_a_ring_nfd . "Bc" . $char_a_ring_nfd, 2, 3 ),
    );

    foreach( $tests as $test ) {
        $arg1 = urlencode($test[1]);
        $arg0 = urlencode($test[0]);
        $res_str .= "find \"$arg1\" in \"$arg0\" - grapheme_strripos";
        if ( 3 == count( $test ) ) {
            $result = grapheme_strripos($test[0], $test[1]);
        }
        else {
            $res_str .= " from $test[2]";
            $result = grapheme_strripos($test[0], $test[1], $test[2]);
        }
        $res_str .= " = ";
        if ( $result === false ) {
            $res_str .= 'false';
        }
        else {
            $res_str .= $result;
        }
        $res_str .= " == " . $test[count($test)-1] . check_result($result, $test[count($test)-1]) . "\n";
    }


    //=====================================================================================
    $res_str .= "\n" . 'function grapheme_substr($string, $start, $length = -1) {}' . "\n\n";

    $tests = array(

        array( "abc", 3, "" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd, 5, "" ),
        array( "ao" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "O", 2, $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "O" ),
        array( $char_o_diaeresis_nfd . $char_a_ring_nfd . "a" . $char_A_ring_nfd . "bc", 2, "a" . $char_A_ring_nfd . "bc" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "O", 5, "O" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd, 5, "" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_O_diaeresis_nfd, 4, $char_O_diaeresis_nfd ),
        array( $char_o_diaeresis_nfd . "a" . $char_a_ring_nfd . "bc", 2, $char_a_ring_nfd . "bc" ),
        array( "a" . $char_A_ring_nfd . "bc", 1, $char_A_ring_nfd . "bc" ),
        array( "Abc", -5, "Abc" ),
        array( $char_a_ring_nfd . "bc", 3, "" ),
        array( "abc", 4, "" ),
        array( "abC", 2, "C" ),
        array( "abc", 1, "bc" ),
        array( "Abc", 1, 1, "b" ),
        array( "abc", 0, 2, "ab" ),
        array( "Abc", -4, 1, "A" ),
        array( "ababc", 1, 2, "ba" ),
        array( "ababc", 0, 10, "ababc" ),

        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", 0, 10 , "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", 5, "Opq" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", 5, -1, "Op" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", 5, -2, "O" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", 5, -3, "" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", 5, -4, "" ),

        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", 0, "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", 0, -1, "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Op" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", 0, -2, "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "O" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", 0, -3, "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", 0, -4, "a" . $char_a_ring_nfd . "bc" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", 0, -5, "a" . $char_a_ring_nfd . "b" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", 0, -6, "a" . $char_a_ring_nfd ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", 0, -7, "a" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", 0, -8, "" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", 0, -9, "" ),

        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -8, "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -7, $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -6, "bc" . $char_o_diaeresis_nfd . "Opq" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -5, "c" . $char_o_diaeresis_nfd . "Opq" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -4, $char_o_diaeresis_nfd . "Opq" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -3, "Opq" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -2, "pq" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -1, "q" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -999, "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq" ),

        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -8, 8, "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -8, 7, "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Op" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -8, 6, "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "O" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -8, 5, "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -8, 4, "a" . $char_a_ring_nfd . "bc" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -8, 3, "a" . $char_a_ring_nfd . "b" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -8, 2, "a" . $char_a_ring_nfd ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -8, 1, "a" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -8, 0, "" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -8, -999, "" ),

        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -8, -1, "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Op" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -8, -2, "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "O" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -8, -3, "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -8, -4, "a" . $char_a_ring_nfd . "bc" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -8, -5, "a" . $char_a_ring_nfd . "b" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -8, -6, "a" . $char_a_ring_nfd ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -8, -7, "a" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -8, -8, "" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "Opq", -8, -9, "" ),

    );

    foreach( $tests as $test ) {
        try {
            $arg0 = urlencode($test[0]);
            $res_str .= "substring of \"$arg0\" from \"$test[1]\" - grapheme_substr";
            if ( 3 == count( $test ) ) {
                $result = grapheme_substr($test[0], $test[1]);
            }
            else {
                $res_str .= " with length $test[2]";
                $result = grapheme_substr($test[0], $test[1], $test[2]);
            }
            $res_str .= " = ";
            if ( $result === false ) {
                $res_str .= 'false';
            }
            else {
                $res_str .= urlencode($result);
            }
            $res_str .= " == " . urlencode($test[count($test)-1]) . check_result($result, $test[count($test)-1]) . "\n";
        } catch (ValueError $exception) {
             $res_str .= ": " . $exception->getMessage() . "\n";
        }
    }


    //=====================================================================================
    $res_str .= "\n" . 'function grapheme_strstr($haystack, $needle, $before_needle = FALSE) {}' . "\n\n";

    $tests = array(
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "o", "o", "o" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd, "o", "false" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd, $char_o_diaeresis_nfd, $char_o_diaeresis_nfd ),
        array( $char_o_diaeresis_nfd . "a" . $char_a_ring_nfd . "bc", $char_a_ring_nfd, $char_a_ring_nfd . "bc"),
        array( "a" . $char_a_ring_nfd . "bc", $char_a_ring_nfd, $char_a_ring_nfd . "bc"),
        array( "abc", $char_a_ring_nfd, "false" ),
        array( $char_a_ring_nfd . "bc", "a", "false" ),
        array( "abc", "d", "false" ),
        array( "abc", "c", "c" ),
        array( "abc", "b", "bc" ),
        array( "abc", "a", "abc" ),
        array( "abc", "ab", "abc" ),
        array( "abc", "abc", "abc" ),
        array( "abc", "bc", "bc" ),
        array( "abc", "a", FALSE, "abc" ),
        array( "abc", "a", TRUE, "" ),
        array( "abc", "b", TRUE, "a" ),
        array( "abc", "c", TRUE, "ab" ),
        array( "ababc", "bab", TRUE, "a" ),
        array( "ababc", "abc", TRUE, "ab" ),
        array( "ababc", "abc", FALSE, "abc" ),

        array( "ab" . $char_a_ring_nfd . "c", "d", "false" ),
        array( "bc" . $char_a_ring_nfd . "a", "a", "a" ),
        array( "a" . $char_a_ring_nfd . "bc", "b", "bc" ),
        array( $char_a_ring_nfd . "bc", "a", "false" ),
        array( $char_a_ring_nfd . "abc", "ab", "abc" ),
        array( "abc" . $char_a_ring_nfd, "abc", "abc" . $char_a_ring_nfd),
        array( "a" . $char_a_ring_nfd . "bc", $char_a_ring_nfd . "bc", $char_a_ring_nfd . "bc" ),
        array( "a" . $char_a_ring_nfd . "bc", $char_a_ring_nfd, FALSE, $char_a_ring_nfd . "bc"),
        array( "a" . $char_a_ring_nfd . "bc", "a", TRUE, "" ),
        array( $char_a_ring_nfd . "abc", "b", TRUE, $char_a_ring_nfd . "a" ),
        array( "ab" . $char_a_ring_nfd . "c", "c", TRUE, "ab" . $char_a_ring_nfd ),
        array( "aba" . $char_a_ring_nfd . "bc", "ba" . $char_a_ring_nfd . "b", TRUE, "a" ),
        array( "ababc" . $char_a_ring_nfd, "abc" . $char_a_ring_nfd, TRUE, "ab" ),
        array( "abab" . $char_a_ring_nfd . "c", "ab" . $char_a_ring_nfd . "c", FALSE, "ab" . $char_a_ring_nfd . "c" ),

    );

    foreach( $tests as $test ) {
        $arg1 = urlencode($test[1]);
        $arg0 = urlencode($test[0]);
        $res_str .= "find \"$arg1\" in \"$arg0\" - grapheme_strstr";
        if ( 3 == count( $test ) ) {
            $result = grapheme_strstr($test[0], $test[1]);
        }
        else {
            $res_str .= " before flag is " . ( $test[2] ? "TRUE" : "FALSE" );
            $result = grapheme_strstr($test[0], $test[1], $test[2]);
        }
        $res_str .= " = ";
        if ( $result === false ) {
            $res_str .= 'false';
        }
        else {
            $res_str .= urlencode($result);
        }
        $res_str .= " == " . urlencode($test[count($test)-1]) . check_result($result, $test[count($test)-1]) . "\n";
    }


    //=====================================================================================
    $res_str .= "\n" . 'function grapheme_stristr($haystack, $needle, $before_needle = FALSE) {}' . "\n\n";

    $tests = array(
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd, $char_O_diaeresis_nfd, $char_o_diaeresis_nfd ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd . "O", "o", "O" ),
        array( "a" . $char_a_ring_nfd . "bc" . $char_o_diaeresis_nfd, "o", "false" ),
        array( $char_o_diaeresis_nfd . "a" . $char_a_ring_nfd . "bc", $char_a_ring_nfd, $char_a_ring_nfd . "bc"),
        array( "a" . $char_a_ring_nfd . "bc", $char_A_ring_nfd, $char_a_ring_nfd . "bc"),
        array( "abc", $char_a_ring_nfd, "false" ),
        array( $char_a_ring_nfd . "bc", "A", "false" ),
        array( "abc", "d", "false" ),
        array( "abc", "C", "c" ),
        array( "aBc", "b", "Bc" ),
        array( "abc", "A", "abc" ),
        array( "abC", "ab", "abC" ),
        array( "abc", "aBc", "abc" ),
        array( "abC", "bc", "bC" ),
        array( "abc", "A", FALSE, "abc" ),
        array( "abc", "a", TRUE, "" ),
        array( "aBc", "b", TRUE, "a" ),
        array( "abc", "C", TRUE, "ab" ),
        array( "aBabc", "bab", TRUE, "a" ),
        array( "ababc", "aBc", TRUE, "ab" ),
        array( "ababc", "abC", FALSE, "abc" ),

        array( "ab" . $char_a_ring_nfd . "c", "d", "false" ),
        array( "bc" . $char_a_ring_nfd . "A", "a", "A" ),
        array( "a" . $char_a_ring_nfd . "bc", "B", "bc" ),
        array( $char_A_ring_nfd . "bc", "a", "false" ),
        array( $char_a_ring_nfd . "abc", "Ab", "abc" ),
        array( "abc" . $char_A_ring_nfd, "abc", "abc" . $char_A_ring_nfd),
        array( "a" . $char_a_ring_nfd . "bc", $char_A_ring_nfd . "bc", $char_a_ring_nfd . "bc" ),
        array( "a" . $char_A_ring_nfd . "bc", $char_a_ring_nfd, FALSE, $char_A_ring_nfd . "bc" ),
        array( "a" . $char_a_ring_nfd . "bc", "A", TRUE, "" ),
        array( $char_a_ring_nfd . "aBc", "b", TRUE, $char_a_ring_nfd . "a" ),
        array( "ab" . $char_a_ring_nfd . "c", "C", TRUE, "ab" . $char_a_ring_nfd ),
        array( "aba" . $char_A_ring_nfd . "bc", "ba" . $char_a_ring_nfd . "b", TRUE, "a" ),
        array( "ababc" . $char_a_ring_nfd, "aBc" . $char_A_ring_nfd, TRUE, "ab" ),
        array( "abAB" . $char_A_ring_nfd . "c", "ab" . $char_a_ring_nfd . "c", FALSE, "AB" . $char_A_ring_nfd . "c" ),

    );

    foreach( $tests as $test ) {
        $arg1 = urlencode($test[1]);
        $arg0 = urlencode($test[0]);
        $res_str .= "find \"$arg1\" in \"$arg0\" - grapheme_stristr";
        if ( 3 == count( $test ) ) {
            $result = grapheme_stristr($test[0], $test[1]);
        }
        else {
            $res_str .= " before flag is " . ( $test[2] ? "TRUE" : "FALSE" );
            $result = grapheme_stristr($test[0], $test[1], $test[2]);
        }
        $res_str .= " = ";
        if ( $result === false ) {
            $res_str .= 'false';
        }
        else {
            $res_str .= urlencode($result);
        }
        $res_str .= " == " . urlencode($test[count($test)-1]) . check_result($result, $test[count($test)-1]) . "\n";
    }


    //=====================================================================================
    $res_str .= "\n" . 'function grapheme_extract($haystack, $size, $extract_type = GRAPHEME_EXTR_COUNT, $start = 0[, $next])' . "\n\n";

    $tests = array(
        // haystack, count, [[offset], [next]], result
        array( "abc", 3, "abc" ),
        array( "abc", 2, "ab" ),
        array( "abc", 1, "a" ),
        array( "abc", 0, "" ),
        array( "abc", 1, 0, "a" ),
        array( "abc", 1, 1, "b" ),
        array( "abc", 1, 2, "c" ),
        array( "abc", 0, 2, "" ),

        array( "abc", 3, 0, 3, "abc" ),
        array( "abc", 2, 0, 2, "ab" ),
        array( "abc", 1, 0, 1, "a" ),
        array( "abc", 0, 0, 0, "" ),
        array( "abc", 1, 0, 1, "a" ),
        array( "abc", 1, 1, 2, "b" ),
        array( "abc", 1, 2, 3, "c" ),
        array( "abc", 1, -2, 2, "b" ),
        array( "abc", 0, 2, 2, "" ),
        array( "http://news.bbc.co.uk/2/hi/middle_east/7831588.stm", 48, 48 , 50 , "tm" ),

        array( $char_a_ring_nfd . "bc", 3, $char_a_ring_nfd . "bc" ),
        array( $char_a_ring_nfd . "bc", 2, $char_a_ring_nfd . "b" ),
        array( $char_a_ring_nfd . "bc", 1, $char_a_ring_nfd . "" ),
        array( $char_a_ring_nfd . "bc", 3, 0, 5, $char_a_ring_nfd . "bc" ),
        array( $char_a_ring_nfd . "bc", 2, 0, 4, $char_a_ring_nfd . "b" ),
        array( $char_a_ring_nfd . "bc", 1, 0, 3, $char_a_ring_nfd . "" ),
        array( $char_a_ring_nfd . "bcde", 2, 3, 5, "bc" ),
        array( $char_a_ring_nfd . "bcde", 2, -4, 5, "bc" ),
        array( $char_a_ring_nfd . "bcde", 2, 4, 6, "cd" ),
        array( $char_a_ring_nfd . "bcde", 2, -7, 4, $char_a_ring_nfd . "b" ),
        array( $char_a_ring_nfd . "bcde" . $char_a_ring_nfd . "f", 4, 5, 11, "de" . $char_a_ring_nfd . "f" ),
        array( $char_a_ring_nfd . "bcde" . $char_a_ring_nfd . "f", 4, -6, 11, "de" . $char_a_ring_nfd . "f" ),

        array( $char_a_ring_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 3, $char_a_ring_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd ),
        array( $char_a_ring_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 2, $char_a_ring_nfd . $char_o_diaeresis_nfd ),
        array( $char_a_ring_nfd . $char_o_diaeresis_nfd . "c", 1, $char_a_ring_nfd . "" ),

        array( $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 1, 0, $char_o_diaeresis_nfd),
        array( $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 1, 2, $char_o_diaeresis_nfd),
        array( $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 1, 3, $char_o_diaeresis_nfd),
        array( $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 1, 4, $char_diaeresis),

        array( $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 2, 0, $char_o_diaeresis_nfd . $char_o_diaeresis_nfd),
        array( $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 2, 2, $char_o_diaeresis_nfd . $char_o_diaeresis_nfd),
        array( $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 2, 3, $char_o_diaeresis_nfd . $char_o_diaeresis_nfd),
        array( $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 2, 4, $char_diaeresis . $char_o_diaeresis_nfd),
        array( $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 2, 7, $char_diaeresis . $char_o_diaeresis_nfd),
        array( $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 2, 8, $char_o_diaeresis_nfd),
        array( $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 2, 10, $char_diaeresis),
        array( $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 2, 11, "false"),

    );

    $next = -1;
    foreach( $tests as $test ) {
        $arg0 = urlencode($test[0]);
        $res_str .= "extract from \"$arg0\" \"$test[1]\" graphemes - grapheme_extract";
        if ( 3 == count( $test ) ) {
            $result = grapheme_extract($test[0], $test[1]);
        }
        elseif ( 4 == count ( $test ) ) {
            $res_str .= " starting at byte position $test[2]";
            $result = grapheme_extract($test[0], $test[1], GRAPHEME_EXTR_COUNT, $test[2]);
        }
        else {
            $res_str .= " starting at byte position $test[2] with \$next";
            $result = grapheme_extract($test[0], $test[1], GRAPHEME_EXTR_COUNT, $test[2], $next);
        }
        $res_str .= " = ";
        if ( $result === false ) {
            $res_str .= 'false';
        }
        else {
            $res_str .= urlencode($result);
        }
        $res_str .= " == " . urlencode($test[count($test)-1]) . check_result($result, $test[count($test)-1]);
        if ( 5 == count ( $test ) ) {
            $res_str .= " \$next=$next == $test[3] ";
            if ( $next != $test[3] ) {
                $res_str .= "***FAILED***";
            }
        }
        $res_str .= "\n";
    }


    //=====================================================================================
    $res_str .= "\n" . 'function grapheme_extract($haystack, $size, $extract_type = GRAPHEME_EXTR_MAXBYTES, $start = 0)' . "\n\n";

    $tests = array(
        array( "abc", 3, "abc" ),
        array( "abc", 2, "ab" ),
        array( "abc", 1, "a" ),
        array( "abc", 0, "" ),
        array( $char_a_ring_nfd . "bc", 5, $char_a_ring_nfd . "bc" ),
        array( $char_a_ring_nfd . "bc", 4, $char_a_ring_nfd . "b" ),
        array( $char_a_ring_nfd . "bc", 1, "" ),
        array( $char_a_ring_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 9, $char_a_ring_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd ),
        array( $char_a_ring_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 10, $char_a_ring_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd ),
        array( $char_a_ring_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 11, $char_a_ring_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd ),
        array( $char_a_ring_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 6, $char_a_ring_nfd . $char_o_diaeresis_nfd ),
        array( $char_a_ring_nfd . $char_o_diaeresis_nfd . "c", 3, $char_a_ring_nfd . "" ),
        array( $char_a_ring_nfd . $char_o_diaeresis_nfd . "c", 4, $char_a_ring_nfd . "" ),
        array( $char_a_ring_nfd . $char_o_diaeresis_nfd . "c", 5, $char_a_ring_nfd . "" ),
        array( $char_a_ring_nfd . $char_o_diaeresis_nfd . "c", 6, $char_a_ring_nfd . $char_o_diaeresis_nfd  ),
        array( $char_a_ring_nfd . $char_o_diaeresis_nfd . "c", 7, $char_a_ring_nfd . $char_o_diaeresis_nfd . "c" ),

        array( $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 3, 0, $char_o_diaeresis_nfd),
        array( $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 3, 2, $char_o_diaeresis_nfd),
        array( $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 3, 3, $char_o_diaeresis_nfd),
        array( $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 3, 4, $char_diaeresis),

        array( $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 6, 0, $char_o_diaeresis_nfd . $char_o_diaeresis_nfd),
        array( $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 6, 2, $char_o_diaeresis_nfd . $char_o_diaeresis_nfd),
        array( $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 6, 3, $char_o_diaeresis_nfd . $char_o_diaeresis_nfd),
        array( $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 5, 4, $char_diaeresis . $char_o_diaeresis_nfd),
        array( $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 5, 7, $char_diaeresis . $char_o_diaeresis_nfd),
        array( $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 3, 8, $char_o_diaeresis_nfd),
        array( $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 2, 10, $char_diaeresis),
        array( $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd . $char_o_diaeresis_nfd, 2, 11, "false"),

    );

    foreach( $tests as $test ) {
        $arg0 = urlencode($test[0]);
        $res_str .= "extract from \"$arg0\" \"$test[1]\" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES";
        if ( 3 == count( $test ) ) {
            $result = grapheme_extract($test[0], $test[1], GRAPHEME_EXTR_MAXBYTES);
        }
        else {
            $res_str .= " starting at byte position $test[2]";
            $result = grapheme_extract($test[0], $test[1], GRAPHEME_EXTR_MAXBYTES, $test[2]);
        }
        $res_str .= " = ";
        if ( $result === false ) {
            $res_str .= 'false';
        }
        else {
            $res_str .= urlencode($result);
        }
        $res_str .= " == " . urlencode($test[count($test)-1]) . check_result($result, $test[count($test)-1]) . "\n";
    }


    //=====================================================================================
    $res_str .= "\n" . 'function grapheme_extract($haystack, $size, $extract_type = GRAPHEME_EXTR_MAXCHARS, $start = 0)' . "\n\n";

    $tests = array(
        array( "abc", 3, "abc" ),
        array( "abc", 2, "ab" ),
        array( "abc", 1, "a" ),
        array( "abc", 0, "" ),
        array( "abc" . $char_o_diaeresis_nfd, 0, "" ),
        array( "abc" . $char_o_diaeresis_nfd, 1, "a" ),
        array( "abc" . $char_o_diaeresis_nfd, 2, "ab" ),
        array( "abc" . $char_o_diaeresis_nfd, 3, "abc" ),
        array( "abc" . $char_o_diaeresis_nfd, 4, "abc" ),
        array( "abc" . $char_o_diaeresis_nfd, 5, "abc" . $char_o_diaeresis_nfd),
        array( "abc" . $char_o_diaeresis_nfd, 6, "abc" . $char_o_diaeresis_nfd),
        array( $char_o_diaeresis_nfd . "abc", 0, "" ),
        array( $char_o_diaeresis_nfd . "abc", 1, "" ),
        array( $char_o_diaeresis_nfd . "abc", 2, $char_o_diaeresis_nfd ),
        array( $char_o_diaeresis_nfd . "abc", 3, $char_o_diaeresis_nfd . "a" ),
        array( $char_o_diaeresis_nfd . "abc", 4, $char_o_diaeresis_nfd . "ab" ),
        array( $char_o_diaeresis_nfd . "abc" . $char_a_ring_nfd . "xyz", 5, $char_o_diaeresis_nfd . "abc" ),
        array( $char_o_diaeresis_nfd . "abc" . $char_a_ring_nfd . "xyz", 6, $char_o_diaeresis_nfd . "abc" ),
        array( $char_o_diaeresis_nfd . "abc" . $char_a_ring_nfd . "xyz", 7, $char_o_diaeresis_nfd . "abc" . $char_a_ring_nfd ),
        array( $char_o_diaeresis_nfd . "abc" . $char_a_ring_nfd . "xyz", 8, $char_o_diaeresis_nfd . "abc" . $char_a_ring_nfd . "x" ),

        array( "abc", 3, 0, "abc" ),
        array( "abc", 2, 1, "bc" ),
        array( "abc", 1, 2, "c" ),
        array( "abc", 0, 3, "false" ),
        array( "abc", 1, 3, "false" ),
        array( "abc", 1, 999, "false" ),
        array( $char_o_diaeresis_nfd . "abc", 1, 6, "false" ),
        array( $char_o_diaeresis_nfd . "abc", 1, 999, "false" ),
        array( $char_o_diaeresis_nfd . "abc" . $char_a_ring_nfd . "xyz", 8, 0, $char_o_diaeresis_nfd . "abc" . $char_a_ring_nfd . "x" ),
        array( $char_o_diaeresis_nfd . "abc" . $char_a_ring_nfd . "xyz", 8, 1, $char_diaeresis . "abc" . $char_a_ring_nfd . "xy" ),
        array( $char_o_diaeresis_nfd . "abc" . $char_a_ring_nfd . "xyz", 8, 2, "abc" . $char_a_ring_nfd . "xyz" ),
        array( $char_o_diaeresis_nfd . "abc" . $char_a_ring_nfd . "xyz", 8, 3, "abc" . $char_a_ring_nfd . "xyz" ),
        array( $char_o_diaeresis_nfd . "abc" . $char_a_ring_nfd . "xyz", 8, 4, "bc" . $char_a_ring_nfd . "xyz" ),
        array( $char_o_diaeresis_nfd . "abc" . $char_a_ring_nfd . "xyz", 8, 5, "c" . $char_a_ring_nfd . "xyz" ),
        array( $char_o_diaeresis_nfd . "abc" . $char_a_ring_nfd . "xyz", 8, 6, $char_a_ring_nfd . "xyz" ),

    );

    foreach( $tests as $test ) {
        $arg0 = urlencode($test[0]);
        $res_str .= "extract from \"$arg0\" \"$test[1]\" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS";
        if ( 3 == count( $test ) ) {
            $result = grapheme_extract($test[0], $test[1], GRAPHEME_EXTR_MAXCHARS);
        }
        else {
            $res_str .= " starting at byte position $test[2]";
            $result = grapheme_extract($test[0], $test[1], GRAPHEME_EXTR_MAXCHARS, $test[2]);
        }
        $res_str .= " = ";
        if ( $result === false ) {
            $res_str .= 'false';
        }
        else {
            $res_str .= urlencode($result);
        }
        $res_str .= " == " . urlencode($test[count($test)-1]) . check_result($result, $test[count($test)-1]) . "\n";
    }


    //=====================================================================================

    return $res_str;
}

echo ut_main();

function check_result($result, $expected) {

    if ( $result === false ) {
            $result = 'false';
    }

    if ( strcmp($result, $expected) != 0 ) {
        return " **FAILED** ";
    }

    return "";
}

?>
--EXPECT--
function grapheme_strlen($string) {}

"hindi" in devanagari strlen 2
"ab" + "hindi" + "cde" strlen 7
"" strlen 0
char_a_ring_nfd strlen 1
char_a_ring_nfd + "bc" strlen 3
"abc" strlen 3

function grapheme_strpos($haystack, $needle, $offset = 0) {}

find "o" in "aa%CC%8Abco%CC%88o" - grapheme_strpos = 5 == 5
find "o" in "aa%CC%8Abco%CC%88" - grapheme_strpos = false == false
find "o%CC%88" in "aa%CC%8Abco%CC%88" - grapheme_strpos = 4 == 4
find "a%CC%8A" in "o%CC%88aa%CC%8Abc" - grapheme_strpos = 2 == 2
find "a%CC%8A" in "aa%CC%8Abc" - grapheme_strpos = 1 == 1
find "a%CC%8A" in "abc" - grapheme_strpos = false == false
find "a" in "a%CC%8Abc" - grapheme_strpos = false == false
find "d" in "abc" - grapheme_strpos = false == false
find "c" in "abc" - grapheme_strpos = 2 == 2
find "b" in "abc" - grapheme_strpos = 1 == 1
find "a" in "abc" - grapheme_strpos = 0 == 0
find "a" in "abc" - grapheme_strpos from 0 = 0 == 0
find "a" in "abc" - grapheme_strpos from 1 = false == false
find "a" in "abc" - grapheme_strpos from -1 = false == false
find "a" in "ababc" - grapheme_strpos from 1 = 2 == 2
find "o" in "aoa%CC%8Abco%CC%88o" - grapheme_strpos from 2 = 6 == 6
find "o" in "aoa%CC%8Abco%CC%88o" - grapheme_strpos from -1 = 6 == 6
find "o" in "aoa%CC%8Abco%CC%88o" - grapheme_strpos from -5 = 6 == 6
find "a%CC%8A" in "o%CC%88a%CC%8Aaa%CC%8Abc" - grapheme_strpos from 2 = 3 == 3
find "a%CC%8A" in "o%CC%88a%CC%8Aaa%CC%8Abc" - grapheme_strpos from -4 = 3 == 3
find "op" in "aa%CC%8Abco%CC%88opq" - grapheme_strpos = 5 == 5
find "opq" in "aa%CC%8Abco%CC%88opq" - grapheme_strpos = 5 == 5
find "abc" in "aa%CC%8Abco%CC%88" - grapheme_strpos = false == false
find "o%CC%88bco%CC%88" in "aa%CC%8Abco%CC%88bco%CC%88" - grapheme_strpos = 4 == 4
find "a%CC%8Abc" in "o%CC%88aa%CC%8Abc" - grapheme_strpos = 2 == 2
find "a%CC%8Abc" in "aa%CC%8Abc" - grapheme_strpos = 1 == 1
find "a%CC%8Abc" in "abc" - grapheme_strpos = false == false
find "abcdefg" in "a%CC%8Abc" - grapheme_strpos = false == false
find "defghijklmnopq" in "abc" - grapheme_strpos = false == false
find "ab" in "abc" - grapheme_strpos = 0 == 0
find "bc" in "abc" - grapheme_strpos = 1 == 1
find "abc" in "abc" - grapheme_strpos = 0 == 0
find "abcd" in "abc" - grapheme_strpos = false == false
find "ab" in "abc" - grapheme_strpos from 0 = 0 == 0
find "abc" in "abc" - grapheme_strpos from 0 = 0 == 0
find "abc" in "abc" - grapheme_strpos from 1 = false == false
find "ab" in "ababc" - grapheme_strpos from 1 = 2 == 2
find "abc" in "ababc" - grapheme_strpos from 1 = 2 == 2
find "oa%CC%8Abc" in "aoa%CC%8Abco%CC%88oa%CC%8Abc" - grapheme_strpos from 2 = 6 == 6
find "oa%CC%8Abc" in "aoa%CC%8Abco%CC%88oa%CC%8Abc" - grapheme_strpos from -8 = 6 == 6
find "a%CC%8Abca%CC%8A" in "o%CC%88a%CC%8Aaa%CC%8Abca%CC%8Adef" - grapheme_strpos from 2 = 3 == 3

function grapheme_stripos($haystack, $needle, $offset = 0) {}

find "o" in "aoa%CC%8Abco%CC%88O" - grapheme_stripos from 2 = 6 == 6
find "o" in "aoa%CC%8Abco%CC%88Oo" - grapheme_stripos from -6 = 6 == 6
find "a%CC%8A" in "o%CC%88a%CC%8AaA%CC%8Abc" - grapheme_stripos from 2 = 3 == 3
find "o" in "aa%CC%8Abco%CC%88O" - grapheme_stripos = 5 == 5
find "O" in "aa%CC%8Abco%CC%88" - grapheme_stripos = false == false
find "o%CC%88" in "aa%CC%8AbcO%CC%88" - grapheme_stripos = 4 == 4
find "o%CC%88" in "aa%CC%8AbcO%CC%88" - grapheme_stripos from -1 = 4 == 4
find "A%CC%8A" in "o%CC%88aa%CC%8Abc" - grapheme_stripos = 2 == 2
find "a%CC%8A" in "aA%CC%8Abc" - grapheme_stripos = 1 == 1
find "a%CC%8A" in "Abc" - grapheme_stripos = false == false
find "A" in "a%CC%8Abc" - grapheme_stripos = false == false
find "D" in "abc" - grapheme_stripos = false == false
find "c" in "abC" - grapheme_stripos = 2 == 2
find "B" in "abc" - grapheme_stripos = 1 == 1
find "a" in "Abc" - grapheme_stripos = 0 == 0
find "A" in "abc" - grapheme_stripos from 0 = 0 == 0
find "a" in "Abc" - grapheme_stripos from 1 = false == false
find "A" in "ababc" - grapheme_stripos from 1 = 2 == 2
find "oP" in "aa%CC%8Abco%CC%88Opq" - grapheme_stripos = 5 == 5
find "opQ" in "aa%CC%8Abco%CC%88Opq" - grapheme_stripos = 5 == 5
find "abc" in "aa%CC%8Abco%CC%88" - grapheme_stripos = false == false
find "O%CC%88bco%CC%88" in "aa%CC%8Abco%CC%88bCo%CC%88" - grapheme_stripos = 4 == 4
find "A%CC%8Abc" in "o%CC%88aa%CC%8ABc" - grapheme_stripos = 2 == 2
find "a%CC%8Abc" in "aa%CC%8ABC" - grapheme_stripos = 1 == 1
find "a%CC%8ABC" in "abc" - grapheme_stripos = false == false
find "aBCdefg" in "a%CC%8ABC" - grapheme_stripos = false == false
find "Defghijklmnopq" in "aBC" - grapheme_stripos = false == false
find "Ab" in "abC" - grapheme_stripos = 0 == 0
find "bc" in "aBC" - grapheme_stripos = 1 == 1
find "Abc" in "abC" - grapheme_stripos = 0 == 0
find "aBcd" in "abC" - grapheme_stripos = false == false
find "ab" in "ABc" - grapheme_stripos from 0 = 0 == 0
find "abC" in "aBc" - grapheme_stripos from 0 = 0 == 0
find "aBc" in "abc" - grapheme_stripos from 1 = false == false
find "AB" in "ABabc" - grapheme_stripos from 1 = 2 == 2
find "AB" in "ABabc" - grapheme_stripos from -4 = 2 == 2
find "aBc" in "abaBc" - grapheme_stripos from 1 = 2 == 2
find "Oa%CC%8AbC" in "aoa%CC%8Abco%CC%88oA%CC%8AbC" - grapheme_stripos from 2 = 6 == 6
find "a%CC%8ABca%CC%8A" in "o%CC%88a%CC%8AaA%CC%8AbCa%CC%8Adef" - grapheme_stripos from 2 = 3 == 3

function grapheme_strrpos($haystack, $needle, $offset = 0) {}

find "o" in "aa%CC%8Abco%CC%88o" - grapheme_strrpos = 5 == 5
find "o" in "aa%CC%8Abco%CC%88" - grapheme_strrpos = false == false
find "o%CC%88" in "aa%CC%8Abco%CC%88" - grapheme_strrpos = 4 == 4
find "a%CC%8A" in "o%CC%88aa%CC%8Abc" - grapheme_strrpos = 2 == 2
find "a%CC%8A" in "aa%CC%8Abc" - grapheme_strrpos = 1 == 1
find "a%CC%8A" in "abc" - grapheme_strrpos = false == false
find "a" in "a%CC%8Abc" - grapheme_strrpos = false == false
find "d" in "abc" - grapheme_strrpos = false == false
find "c" in "abc" - grapheme_strrpos = 2 == 2
find "b" in "abc" - grapheme_strrpos = 1 == 1
find "a" in "abc" - grapheme_strrpos = 0 == 0
find "a" in "abc" - grapheme_strrpos from 0 = 0 == 0
find "a" in "abc" - grapheme_strrpos from 1 = false == false
find "a" in "ababc" - grapheme_strrpos from 1 = 2 == 2
find "o" in "aoa%CC%8Abco%CC%88o" - grapheme_strrpos from 2 = 6 == 6
find "a%CC%8A" in "o%CC%88a%CC%8Aaa%CC%8Abc" - grapheme_strrpos from 2 = 3 == 3
find "op" in "aa%CC%8Abco%CC%88opq" - grapheme_strrpos = 5 == 5
find "opq" in "aa%CC%8Abco%CC%88opq" - grapheme_strrpos = 5 == 5
find "abc" in "aa%CC%8Abco%CC%88" - grapheme_strrpos = false == false
find "o%CC%88bco%CC%88" in "aa%CC%8Abco%CC%88bco%CC%88" - grapheme_strrpos = 4 == 4
find "a%CC%8Abc" in "o%CC%88aa%CC%8Abc" - grapheme_strrpos = 2 == 2
find "a%CC%8Abc" in "aa%CC%8Abc" - grapheme_strrpos = 1 == 1
find "a%CC%8Abc" in "abc" - grapheme_strrpos = false == false
find "abcdefg" in "a%CC%8Abc" - grapheme_strrpos = false == false
find "defghijklmnopq" in "abc" - grapheme_strrpos = false == false
find "ab" in "abc" - grapheme_strrpos = 0 == 0
find "bc" in "abc" - grapheme_strrpos = 1 == 1
find "abc" in "abc" - grapheme_strrpos = 0 == 0
find "abcd" in "abc" - grapheme_strrpos = false == false
find "ab" in "abc" - grapheme_strrpos from 0 = 0 == 0
find "abc" in "abc" - grapheme_strrpos from 0 = 0 == 0
find "abc" in "abc" - grapheme_strrpos from 1 = false == false
find "ab" in "ababc" - grapheme_strrpos from 1 = 2 == 2
find "abc" in "ababc" - grapheme_strrpos from 1 = 2 == 2
find "oa%CC%8Abc" in "aoa%CC%8Abco%CC%88oa%CC%8Abc" - grapheme_strrpos from 2 = 6 == 6
find "a%CC%8Abca%CC%8A" in "o%CC%88a%CC%8Aaa%CC%8Abca%CC%8Adef" - grapheme_strrpos from 2 = 3 == 3

function grapheme_strripos($haystack, $needle, $offset = 0) {}

find "o" in "aoa%CC%8Abco%CC%88O" - grapheme_strripos from 2 = 6 == 6
find "a%CC%8A" in "o%CC%88a%CC%8AaA%CC%8Abc" - grapheme_strripos from 2 = 3 == 3
find "o" in "aa%CC%8Abco%CC%88O" - grapheme_strripos = 5 == 5
find "O" in "aa%CC%8Abco%CC%88" - grapheme_strripos = false == false
find "o%CC%88" in "aa%CC%8AbcO%CC%88" - grapheme_strripos = 4 == 4
find "A%CC%8A" in "o%CC%88aa%CC%8Abc" - grapheme_strripos = 2 == 2
find "a%CC%8A" in "aA%CC%8Abc" - grapheme_strripos = 1 == 1
find "a%CC%8A" in "Abc" - grapheme_strripos = false == false
find "A" in "a%CC%8Abc" - grapheme_strripos = false == false
find "D" in "abc" - grapheme_strripos = false == false
find "c" in "abC" - grapheme_strripos = 2 == 2
find "B" in "abc" - grapheme_strripos = 1 == 1
find "a" in "Abc" - grapheme_strripos = 0 == 0
find "A" in "abc" - grapheme_strripos from 0 = 0 == 0
find "a" in "Abc" - grapheme_strripos from 1 = false == false
find "A" in "ababc" - grapheme_strripos from 1 = 2 == 2
find "oP" in "aa%CC%8Abco%CC%88Opq" - grapheme_strripos = 5 == 5
find "opQ" in "aa%CC%8Abco%CC%88Opq" - grapheme_strripos = 5 == 5
find "abc" in "aa%CC%8Abco%CC%88" - grapheme_strripos = false == false
find "O%CC%88bco%CC%88" in "aa%CC%8Abco%CC%88bCo%CC%88" - grapheme_strripos = 4 == 4
find "A%CC%8Abc" in "o%CC%88aa%CC%8ABc" - grapheme_strripos = 2 == 2
find "a%CC%8Abc" in "aa%CC%8ABC" - grapheme_strripos = 1 == 1
find "a%CC%8ABC" in "abc" - grapheme_strripos = false == false
find "aBCdefg" in "a%CC%8ABC" - grapheme_strripos = false == false
find "Defghijklmnopq" in "aBC" - grapheme_strripos = false == false
find "Ab" in "abC" - grapheme_strripos = 0 == 0
find "bc" in "aBC" - grapheme_strripos = 1 == 1
find "Abc" in "abC" - grapheme_strripos = 0 == 0
find "aBcd" in "abC" - grapheme_strripos = false == false
find "ab" in "ABc" - grapheme_strripos from 0 = 0 == 0
find "abC" in "aBc" - grapheme_strripos from 0 = 0 == 0
find "aBc" in "abc" - grapheme_strripos from 1 = false == false
find "AB" in "ABabc" - grapheme_strripos from 1 = 2 == 2
find "aBc" in "abaBc" - grapheme_strripos from 1 = 2 == 2
find "Oa%CC%8AbC" in "aoa%CC%8Abco%CC%88oA%CC%8AbC" - grapheme_strripos from 2 = 6 == 6
find "a%CC%8ABca%CC%8A" in "o%CC%88a%CC%8AaA%CC%8AbCa%CC%8Adef" - grapheme_strripos from 2 = 3 == 3

function grapheme_substr($string, $start, $length = -1) {}

substring of "abc" from "3" - grapheme_substr =  == 
substring of "aa%CC%8Abco%CC%88" from "5" - grapheme_substr =  == 
substring of "aoa%CC%8Abco%CC%88O" from "2" - grapheme_substr = a%CC%8Abco%CC%88O == a%CC%8Abco%CC%88O
substring of "o%CC%88a%CC%8AaA%CC%8Abc" from "2" - grapheme_substr = aA%CC%8Abc == aA%CC%8Abc
substring of "aa%CC%8Abco%CC%88O" from "5" - grapheme_substr = O == O
substring of "aa%CC%8Abco%CC%88" from "5" - grapheme_substr =  == 
substring of "aa%CC%8AbcO%CC%88" from "4" - grapheme_substr = O%CC%88 == O%CC%88
substring of "o%CC%88aa%CC%8Abc" from "2" - grapheme_substr = a%CC%8Abc == a%CC%8Abc
substring of "aA%CC%8Abc" from "1" - grapheme_substr = A%CC%8Abc == A%CC%8Abc
substring of "Abc" from "-5" - grapheme_substr = Abc == Abc
substring of "a%CC%8Abc" from "3" - grapheme_substr =  == 
substring of "abc" from "4" - grapheme_substr =  == 
substring of "abC" from "2" - grapheme_substr = C == C
substring of "abc" from "1" - grapheme_substr = bc == bc
substring of "Abc" from "1" - grapheme_substr with length 1 = b == b
substring of "abc" from "0" - grapheme_substr with length 2 = ab == ab
substring of "Abc" from "-4" - grapheme_substr with length 1 = A == A
substring of "ababc" from "1" - grapheme_substr with length 2 = ba == ba
substring of "ababc" from "0" - grapheme_substr with length 10 = ababc == ababc
substring of "aa%CC%8Abco%CC%88Opq" from "0" - grapheme_substr with length 10 = aa%CC%8Abco%CC%88Opq == aa%CC%8Abco%CC%88Opq
substring of "aa%CC%8Abco%CC%88Opq" from "5" - grapheme_substr = Opq == Opq
substring of "aa%CC%8Abco%CC%88Opq" from "5" - grapheme_substr with length -1 = Op == Op
substring of "aa%CC%8Abco%CC%88Opq" from "5" - grapheme_substr with length -2 = O == O
substring of "aa%CC%8Abco%CC%88Opq" from "5" - grapheme_substr with length -3 =  == 
substring of "aa%CC%8Abco%CC%88Opq" from "5" - grapheme_substr with length -4 =  == 
substring of "aa%CC%8Abco%CC%88Opq" from "0" - grapheme_substr = aa%CC%8Abco%CC%88Opq == aa%CC%8Abco%CC%88Opq
substring of "aa%CC%8Abco%CC%88Opq" from "0" - grapheme_substr with length -1 = aa%CC%8Abco%CC%88Op == aa%CC%8Abco%CC%88Op
substring of "aa%CC%8Abco%CC%88Opq" from "0" - grapheme_substr with length -2 = aa%CC%8Abco%CC%88O == aa%CC%8Abco%CC%88O
substring of "aa%CC%8Abco%CC%88Opq" from "0" - grapheme_substr with length -3 = aa%CC%8Abco%CC%88 == aa%CC%8Abco%CC%88
substring of "aa%CC%8Abco%CC%88Opq" from "0" - grapheme_substr with length -4 = aa%CC%8Abc == aa%CC%8Abc
substring of "aa%CC%8Abco%CC%88Opq" from "0" - grapheme_substr with length -5 = aa%CC%8Ab == aa%CC%8Ab
substring of "aa%CC%8Abco%CC%88Opq" from "0" - grapheme_substr with length -6 = aa%CC%8A == aa%CC%8A
substring of "aa%CC%8Abco%CC%88Opq" from "0" - grapheme_substr with length -7 = a == a
substring of "aa%CC%8Abco%CC%88Opq" from "0" - grapheme_substr with length -8 =  == 
substring of "aa%CC%8Abco%CC%88Opq" from "0" - grapheme_substr with length -9 =  == 
substring of "aa%CC%8Abco%CC%88Opq" from "-8" - grapheme_substr = aa%CC%8Abco%CC%88Opq == aa%CC%8Abco%CC%88Opq
substring of "aa%CC%8Abco%CC%88Opq" from "-7" - grapheme_substr = a%CC%8Abco%CC%88Opq == a%CC%8Abco%CC%88Opq
substring of "aa%CC%8Abco%CC%88Opq" from "-6" - grapheme_substr = bco%CC%88Opq == bco%CC%88Opq
substring of "aa%CC%8Abco%CC%88Opq" from "-5" - grapheme_substr = co%CC%88Opq == co%CC%88Opq
substring of "aa%CC%8Abco%CC%88Opq" from "-4" - grapheme_substr = o%CC%88Opq == o%CC%88Opq
substring of "aa%CC%8Abco%CC%88Opq" from "-3" - grapheme_substr = Opq == Opq
substring of "aa%CC%8Abco%CC%88Opq" from "-2" - grapheme_substr = pq == pq
substring of "aa%CC%8Abco%CC%88Opq" from "-1" - grapheme_substr = q == q
substring of "aa%CC%8Abco%CC%88Opq" from "-999" - grapheme_substr = aa%CC%8Abco%CC%88Opq == aa%CC%8Abco%CC%88Opq
substring of "aa%CC%8Abco%CC%88Opq" from "-8" - grapheme_substr with length 8 = aa%CC%8Abco%CC%88Opq == aa%CC%8Abco%CC%88Opq
substring of "aa%CC%8Abco%CC%88Opq" from "-8" - grapheme_substr with length 7 = aa%CC%8Abco%CC%88Op == aa%CC%8Abco%CC%88Op
substring of "aa%CC%8Abco%CC%88Opq" from "-8" - grapheme_substr with length 6 = aa%CC%8Abco%CC%88O == aa%CC%8Abco%CC%88O
substring of "aa%CC%8Abco%CC%88Opq" from "-8" - grapheme_substr with length 5 = aa%CC%8Abco%CC%88 == aa%CC%8Abco%CC%88
substring of "aa%CC%8Abco%CC%88Opq" from "-8" - grapheme_substr with length 4 = aa%CC%8Abc == aa%CC%8Abc
substring of "aa%CC%8Abco%CC%88Opq" from "-8" - grapheme_substr with length 3 = aa%CC%8Ab == aa%CC%8Ab
substring of "aa%CC%8Abco%CC%88Opq" from "-8" - grapheme_substr with length 2 = aa%CC%8A == aa%CC%8A
substring of "aa%CC%8Abco%CC%88Opq" from "-8" - grapheme_substr with length 1 = a == a
substring of "aa%CC%8Abco%CC%88Opq" from "-8" - grapheme_substr with length 0 =  == 
substring of "aa%CC%8Abco%CC%88Opq" from "-8" - grapheme_substr with length -999 =  == 
substring of "aa%CC%8Abco%CC%88Opq" from "-8" - grapheme_substr with length -1 = aa%CC%8Abco%CC%88Op == aa%CC%8Abco%CC%88Op
substring of "aa%CC%8Abco%CC%88Opq" from "-8" - grapheme_substr with length -2 = aa%CC%8Abco%CC%88O == aa%CC%8Abco%CC%88O
substring of "aa%CC%8Abco%CC%88Opq" from "-8" - grapheme_substr with length -3 = aa%CC%8Abco%CC%88 == aa%CC%8Abco%CC%88
substring of "aa%CC%8Abco%CC%88Opq" from "-8" - grapheme_substr with length -4 = aa%CC%8Abc == aa%CC%8Abc
substring of "aa%CC%8Abco%CC%88Opq" from "-8" - grapheme_substr with length -5 = aa%CC%8Ab == aa%CC%8Ab
substring of "aa%CC%8Abco%CC%88Opq" from "-8" - grapheme_substr with length -6 = aa%CC%8A == aa%CC%8A
substring of "aa%CC%8Abco%CC%88Opq" from "-8" - grapheme_substr with length -7 = a == a
substring of "aa%CC%8Abco%CC%88Opq" from "-8" - grapheme_substr with length -8 =  == 
substring of "aa%CC%8Abco%CC%88Opq" from "-8" - grapheme_substr with length -9 =  == 

function grapheme_strstr($haystack, $needle, $before_needle = FALSE) {}

find "o" in "aa%CC%8Abco%CC%88o" - grapheme_strstr = o == o
find "o" in "aa%CC%8Abco%CC%88" - grapheme_strstr = false == false
find "o%CC%88" in "aa%CC%8Abco%CC%88" - grapheme_strstr = o%CC%88 == o%CC%88
find "a%CC%8A" in "o%CC%88aa%CC%8Abc" - grapheme_strstr = a%CC%8Abc == a%CC%8Abc
find "a%CC%8A" in "aa%CC%8Abc" - grapheme_strstr = a%CC%8Abc == a%CC%8Abc
find "a%CC%8A" in "abc" - grapheme_strstr = false == false
find "a" in "a%CC%8Abc" - grapheme_strstr = false == false
find "d" in "abc" - grapheme_strstr = false == false
find "c" in "abc" - grapheme_strstr = c == c
find "b" in "abc" - grapheme_strstr = bc == bc
find "a" in "abc" - grapheme_strstr = abc == abc
find "ab" in "abc" - grapheme_strstr = abc == abc
find "abc" in "abc" - grapheme_strstr = abc == abc
find "bc" in "abc" - grapheme_strstr = bc == bc
find "a" in "abc" - grapheme_strstr before flag is FALSE = abc == abc
find "a" in "abc" - grapheme_strstr before flag is TRUE =  == 
find "b" in "abc" - grapheme_strstr before flag is TRUE = a == a
find "c" in "abc" - grapheme_strstr before flag is TRUE = ab == ab
find "bab" in "ababc" - grapheme_strstr before flag is TRUE = a == a
find "abc" in "ababc" - grapheme_strstr before flag is TRUE = ab == ab
find "abc" in "ababc" - grapheme_strstr before flag is FALSE = abc == abc
find "d" in "aba%CC%8Ac" - grapheme_strstr = false == false
find "a" in "bca%CC%8Aa" - grapheme_strstr = a == a
find "b" in "aa%CC%8Abc" - grapheme_strstr = bc == bc
find "a" in "a%CC%8Abc" - grapheme_strstr = false == false
find "ab" in "a%CC%8Aabc" - grapheme_strstr = abc == abc
find "abc" in "abca%CC%8A" - grapheme_strstr = abca%CC%8A == abca%CC%8A
find "a%CC%8Abc" in "aa%CC%8Abc" - grapheme_strstr = a%CC%8Abc == a%CC%8Abc
find "a%CC%8A" in "aa%CC%8Abc" - grapheme_strstr before flag is FALSE = a%CC%8Abc == a%CC%8Abc
find "a" in "aa%CC%8Abc" - grapheme_strstr before flag is TRUE =  == 
find "b" in "a%CC%8Aabc" - grapheme_strstr before flag is TRUE = a%CC%8Aa == a%CC%8Aa
find "c" in "aba%CC%8Ac" - grapheme_strstr before flag is TRUE = aba%CC%8A == aba%CC%8A
find "baa%CC%8Ab" in "abaa%CC%8Abc" - grapheme_strstr before flag is TRUE = a == a
find "abca%CC%8A" in "ababca%CC%8A" - grapheme_strstr before flag is TRUE = ab == ab
find "aba%CC%8Ac" in "ababa%CC%8Ac" - grapheme_strstr before flag is FALSE = aba%CC%8Ac == aba%CC%8Ac

function grapheme_stristr($haystack, $needle, $before_needle = FALSE) {}

find "O%CC%88" in "aa%CC%8Abco%CC%88" - grapheme_stristr = o%CC%88 == o%CC%88
find "o" in "aa%CC%8Abco%CC%88O" - grapheme_stristr = O == O
find "o" in "aa%CC%8Abco%CC%88" - grapheme_stristr = false == false
find "a%CC%8A" in "o%CC%88aa%CC%8Abc" - grapheme_stristr = a%CC%8Abc == a%CC%8Abc
find "A%CC%8A" in "aa%CC%8Abc" - grapheme_stristr = a%CC%8Abc == a%CC%8Abc
find "a%CC%8A" in "abc" - grapheme_stristr = false == false
find "A" in "a%CC%8Abc" - grapheme_stristr = false == false
find "d" in "abc" - grapheme_stristr = false == false
find "C" in "abc" - grapheme_stristr = c == c
find "b" in "aBc" - grapheme_stristr = Bc == Bc
find "A" in "abc" - grapheme_stristr = abc == abc
find "ab" in "abC" - grapheme_stristr = abC == abC
find "aBc" in "abc" - grapheme_stristr = abc == abc
find "bc" in "abC" - grapheme_stristr = bC == bC
find "A" in "abc" - grapheme_stristr before flag is FALSE = abc == abc
find "a" in "abc" - grapheme_stristr before flag is TRUE =  == 
find "b" in "aBc" - grapheme_stristr before flag is TRUE = a == a
find "C" in "abc" - grapheme_stristr before flag is TRUE = ab == ab
find "bab" in "aBabc" - grapheme_stristr before flag is TRUE = a == a
find "aBc" in "ababc" - grapheme_stristr before flag is TRUE = ab == ab
find "abC" in "ababc" - grapheme_stristr before flag is FALSE = abc == abc
find "d" in "aba%CC%8Ac" - grapheme_stristr = false == false
find "a" in "bca%CC%8AA" - grapheme_stristr = A == A
find "B" in "aa%CC%8Abc" - grapheme_stristr = bc == bc
find "a" in "A%CC%8Abc" - grapheme_stristr = false == false
find "Ab" in "a%CC%8Aabc" - grapheme_stristr = abc == abc
find "abc" in "abcA%CC%8A" - grapheme_stristr = abcA%CC%8A == abcA%CC%8A
find "A%CC%8Abc" in "aa%CC%8Abc" - grapheme_stristr = a%CC%8Abc == a%CC%8Abc
find "a%CC%8A" in "aA%CC%8Abc" - grapheme_stristr before flag is FALSE = A%CC%8Abc == A%CC%8Abc
find "A" in "aa%CC%8Abc" - grapheme_stristr before flag is TRUE =  == 
find "b" in "a%CC%8AaBc" - grapheme_stristr before flag is TRUE = a%CC%8Aa == a%CC%8Aa
find "C" in "aba%CC%8Ac" - grapheme_stristr before flag is TRUE = aba%CC%8A == aba%CC%8A
find "baa%CC%8Ab" in "abaA%CC%8Abc" - grapheme_stristr before flag is TRUE = a == a
find "aBcA%CC%8A" in "ababca%CC%8A" - grapheme_stristr before flag is TRUE = ab == ab
find "aba%CC%8Ac" in "abABA%CC%8Ac" - grapheme_stristr before flag is FALSE = ABA%CC%8Ac == ABA%CC%8Ac

function grapheme_extract($haystack, $size, $extract_type = GRAPHEME_EXTR_COUNT, $start = 0[, $next])

extract from "abc" "3" graphemes - grapheme_extract = abc == abc
extract from "abc" "2" graphemes - grapheme_extract = ab == ab
extract from "abc" "1" graphemes - grapheme_extract = a == a
extract from "abc" "0" graphemes - grapheme_extract =  == 
extract from "abc" "1" graphemes - grapheme_extract starting at byte position 0 = a == a
extract from "abc" "1" graphemes - grapheme_extract starting at byte position 1 = b == b
extract from "abc" "1" graphemes - grapheme_extract starting at byte position 2 = c == c
extract from "abc" "0" graphemes - grapheme_extract starting at byte position 2 =  == 
extract from "abc" "3" graphemes - grapheme_extract starting at byte position 0 with $next = abc == abc $next=3 == 3 
extract from "abc" "2" graphemes - grapheme_extract starting at byte position 0 with $next = ab == ab $next=2 == 2 
extract from "abc" "1" graphemes - grapheme_extract starting at byte position 0 with $next = a == a $next=1 == 1 
extract from "abc" "0" graphemes - grapheme_extract starting at byte position 0 with $next =  ==  $next=0 == 0 
extract from "abc" "1" graphemes - grapheme_extract starting at byte position 0 with $next = a == a $next=1 == 1 
extract from "abc" "1" graphemes - grapheme_extract starting at byte position 1 with $next = b == b $next=2 == 2 
extract from "abc" "1" graphemes - grapheme_extract starting at byte position 2 with $next = c == c $next=3 == 3 
extract from "abc" "1" graphemes - grapheme_extract starting at byte position -2 with $next = b == b $next=2 == 2 
extract from "abc" "0" graphemes - grapheme_extract starting at byte position 2 with $next =  ==  $next=2 == 2 
extract from "http%3A%2F%2Fnews.bbc.co.uk%2F2%2Fhi%2Fmiddle_east%2F7831588.stm" "48" graphemes - grapheme_extract starting at byte position 48 with $next = tm == tm $next=50 == 50 
extract from "a%CC%8Abc" "3" graphemes - grapheme_extract = a%CC%8Abc == a%CC%8Abc
extract from "a%CC%8Abc" "2" graphemes - grapheme_extract = a%CC%8Ab == a%CC%8Ab
extract from "a%CC%8Abc" "1" graphemes - grapheme_extract = a%CC%8A == a%CC%8A
extract from "a%CC%8Abc" "3" graphemes - grapheme_extract starting at byte position 0 with $next = a%CC%8Abc == a%CC%8Abc $next=5 == 5 
extract from "a%CC%8Abc" "2" graphemes - grapheme_extract starting at byte position 0 with $next = a%CC%8Ab == a%CC%8Ab $next=4 == 4 
extract from "a%CC%8Abc" "1" graphemes - grapheme_extract starting at byte position 0 with $next = a%CC%8A == a%CC%8A $next=3 == 3 
extract from "a%CC%8Abcde" "2" graphemes - grapheme_extract starting at byte position 3 with $next = bc == bc $next=5 == 5 
extract from "a%CC%8Abcde" "2" graphemes - grapheme_extract starting at byte position -4 with $next = bc == bc $next=5 == 5 
extract from "a%CC%8Abcde" "2" graphemes - grapheme_extract starting at byte position 4 with $next = cd == cd $next=6 == 6 
extract from "a%CC%8Abcde" "2" graphemes - grapheme_extract starting at byte position -7 with $next = a%CC%8Ab == a%CC%8Ab $next=4 == 4 
extract from "a%CC%8Abcdea%CC%8Af" "4" graphemes - grapheme_extract starting at byte position 5 with $next = dea%CC%8Af == dea%CC%8Af $next=11 == 11 
extract from "a%CC%8Abcdea%CC%8Af" "4" graphemes - grapheme_extract starting at byte position -6 with $next = dea%CC%8Af == dea%CC%8Af $next=11 == 11 
extract from "a%CC%8Ao%CC%88o%CC%88" "3" graphemes - grapheme_extract = a%CC%8Ao%CC%88o%CC%88 == a%CC%8Ao%CC%88o%CC%88
extract from "a%CC%8Ao%CC%88o%CC%88" "2" graphemes - grapheme_extract = a%CC%8Ao%CC%88 == a%CC%8Ao%CC%88
extract from "a%CC%8Ao%CC%88c" "1" graphemes - grapheme_extract = a%CC%8A == a%CC%8A
extract from "o%CC%88o%CC%88o%CC%88o%CC%88" "1" graphemes - grapheme_extract starting at byte position 0 = o%CC%88 == o%CC%88
extract from "o%CC%88o%CC%88o%CC%88o%CC%88" "1" graphemes - grapheme_extract starting at byte position 2 = o%CC%88 == o%CC%88
extract from "o%CC%88o%CC%88o%CC%88o%CC%88" "1" graphemes - grapheme_extract starting at byte position 3 = o%CC%88 == o%CC%88
extract from "o%CC%88o%CC%88o%CC%88o%CC%88" "1" graphemes - grapheme_extract starting at byte position 4 = %CC%88 == %CC%88
extract from "o%CC%88o%CC%88o%CC%88o%CC%88" "2" graphemes - grapheme_extract starting at byte position 0 = o%CC%88o%CC%88 == o%CC%88o%CC%88
extract from "o%CC%88o%CC%88o%CC%88o%CC%88" "2" graphemes - grapheme_extract starting at byte position 2 = o%CC%88o%CC%88 == o%CC%88o%CC%88
extract from "o%CC%88o%CC%88o%CC%88o%CC%88" "2" graphemes - grapheme_extract starting at byte position 3 = o%CC%88o%CC%88 == o%CC%88o%CC%88
extract from "o%CC%88o%CC%88o%CC%88o%CC%88" "2" graphemes - grapheme_extract starting at byte position 4 = %CC%88o%CC%88 == %CC%88o%CC%88
extract from "o%CC%88o%CC%88o%CC%88o%CC%88" "2" graphemes - grapheme_extract starting at byte position 7 = %CC%88o%CC%88 == %CC%88o%CC%88
extract from "o%CC%88o%CC%88o%CC%88o%CC%88" "2" graphemes - grapheme_extract starting at byte position 8 = o%CC%88 == o%CC%88
extract from "o%CC%88o%CC%88o%CC%88o%CC%88" "2" graphemes - grapheme_extract starting at byte position 10 = %CC%88 == %CC%88
extract from "o%CC%88o%CC%88o%CC%88o%CC%88" "2" graphemes - grapheme_extract starting at byte position 11 = false == false

function grapheme_extract($haystack, $size, $extract_type = GRAPHEME_EXTR_MAXBYTES, $start = 0)

extract from "abc" "3" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES = abc == abc
extract from "abc" "2" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES = ab == ab
extract from "abc" "1" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES = a == a
extract from "abc" "0" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES =  == 
extract from "a%CC%8Abc" "5" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES = a%CC%8Abc == a%CC%8Abc
extract from "a%CC%8Abc" "4" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES = a%CC%8Ab == a%CC%8Ab
extract from "a%CC%8Abc" "1" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES =  == 
extract from "a%CC%8Ao%CC%88o%CC%88" "9" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES = a%CC%8Ao%CC%88o%CC%88 == a%CC%8Ao%CC%88o%CC%88
extract from "a%CC%8Ao%CC%88o%CC%88" "10" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES = a%CC%8Ao%CC%88o%CC%88 == a%CC%8Ao%CC%88o%CC%88
extract from "a%CC%8Ao%CC%88o%CC%88" "11" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES = a%CC%8Ao%CC%88o%CC%88 == a%CC%8Ao%CC%88o%CC%88
extract from "a%CC%8Ao%CC%88o%CC%88" "6" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES = a%CC%8Ao%CC%88 == a%CC%8Ao%CC%88
extract from "a%CC%8Ao%CC%88c" "3" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES = a%CC%8A == a%CC%8A
extract from "a%CC%8Ao%CC%88c" "4" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES = a%CC%8A == a%CC%8A
extract from "a%CC%8Ao%CC%88c" "5" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES = a%CC%8A == a%CC%8A
extract from "a%CC%8Ao%CC%88c" "6" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES = a%CC%8Ao%CC%88 == a%CC%8Ao%CC%88
extract from "a%CC%8Ao%CC%88c" "7" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES = a%CC%8Ao%CC%88c == a%CC%8Ao%CC%88c
extract from "o%CC%88o%CC%88o%CC%88o%CC%88" "3" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES starting at byte position 0 = o%CC%88 == o%CC%88
extract from "o%CC%88o%CC%88o%CC%88o%CC%88" "3" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES starting at byte position 2 = o%CC%88 == o%CC%88
extract from "o%CC%88o%CC%88o%CC%88o%CC%88" "3" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES starting at byte position 3 = o%CC%88 == o%CC%88
extract from "o%CC%88o%CC%88o%CC%88o%CC%88" "3" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES starting at byte position 4 = %CC%88 == %CC%88
extract from "o%CC%88o%CC%88o%CC%88o%CC%88" "6" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES starting at byte position 0 = o%CC%88o%CC%88 == o%CC%88o%CC%88
extract from "o%CC%88o%CC%88o%CC%88o%CC%88" "6" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES starting at byte position 2 = o%CC%88o%CC%88 == o%CC%88o%CC%88
extract from "o%CC%88o%CC%88o%CC%88o%CC%88" "6" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES starting at byte position 3 = o%CC%88o%CC%88 == o%CC%88o%CC%88
extract from "o%CC%88o%CC%88o%CC%88o%CC%88" "5" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES starting at byte position 4 = %CC%88o%CC%88 == %CC%88o%CC%88
extract from "o%CC%88o%CC%88o%CC%88o%CC%88" "5" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES starting at byte position 7 = %CC%88o%CC%88 == %CC%88o%CC%88
extract from "o%CC%88o%CC%88o%CC%88o%CC%88" "3" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES starting at byte position 8 = o%CC%88 == o%CC%88
extract from "o%CC%88o%CC%88o%CC%88o%CC%88" "2" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES starting at byte position 10 = %CC%88 == %CC%88
extract from "o%CC%88o%CC%88o%CC%88o%CC%88" "2" graphemes - grapheme_extract GRAPHEME_EXTR_MAXBYTES starting at byte position 11 = false == false

function grapheme_extract($haystack, $size, $extract_type = GRAPHEME_EXTR_MAXCHARS, $start = 0)

extract from "abc" "3" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS = abc == abc
extract from "abc" "2" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS = ab == ab
extract from "abc" "1" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS = a == a
extract from "abc" "0" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS =  == 
extract from "abco%CC%88" "0" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS =  == 
extract from "abco%CC%88" "1" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS = a == a
extract from "abco%CC%88" "2" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS = ab == ab
extract from "abco%CC%88" "3" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS = abc == abc
extract from "abco%CC%88" "4" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS = abc == abc
extract from "abco%CC%88" "5" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS = abco%CC%88 == abco%CC%88
extract from "abco%CC%88" "6" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS = abco%CC%88 == abco%CC%88
extract from "o%CC%88abc" "0" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS =  == 
extract from "o%CC%88abc" "1" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS =  == 
extract from "o%CC%88abc" "2" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS = o%CC%88 == o%CC%88
extract from "o%CC%88abc" "3" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS = o%CC%88a == o%CC%88a
extract from "o%CC%88abc" "4" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS = o%CC%88ab == o%CC%88ab
extract from "o%CC%88abca%CC%8Axyz" "5" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS = o%CC%88abc == o%CC%88abc
extract from "o%CC%88abca%CC%8Axyz" "6" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS = o%CC%88abc == o%CC%88abc
extract from "o%CC%88abca%CC%8Axyz" "7" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS = o%CC%88abca%CC%8A == o%CC%88abca%CC%8A
extract from "o%CC%88abca%CC%8Axyz" "8" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS = o%CC%88abca%CC%8Ax == o%CC%88abca%CC%8Ax
extract from "abc" "3" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS starting at byte position 0 = abc == abc
extract from "abc" "2" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS starting at byte position 1 = bc == bc
extract from "abc" "1" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS starting at byte position 2 = c == c
extract from "abc" "0" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS starting at byte position 3 = false == false
extract from "abc" "1" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS starting at byte position 3 = false == false
extract from "abc" "1" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS starting at byte position 999 = false == false
extract from "o%CC%88abc" "1" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS starting at byte position 6 = false == false
extract from "o%CC%88abc" "1" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS starting at byte position 999 = false == false
extract from "o%CC%88abca%CC%8Axyz" "8" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS starting at byte position 0 = o%CC%88abca%CC%8Ax == o%CC%88abca%CC%8Ax
extract from "o%CC%88abca%CC%8Axyz" "8" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS starting at byte position 1 = %CC%88abca%CC%8Axy == %CC%88abca%CC%8Axy
extract from "o%CC%88abca%CC%8Axyz" "8" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS starting at byte position 2 = abca%CC%8Axyz == abca%CC%8Axyz
extract from "o%CC%88abca%CC%8Axyz" "8" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS starting at byte position 3 = abca%CC%8Axyz == abca%CC%8Axyz
extract from "o%CC%88abca%CC%8Axyz" "8" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS starting at byte position 4 = bca%CC%8Axyz == bca%CC%8Axyz
extract from "o%CC%88abca%CC%8Axyz" "8" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS starting at byte position 5 = ca%CC%8Axyz == ca%CC%8Axyz
extract from "o%CC%88abca%CC%8Axyz" "8" graphemes - grapheme_extract GRAPHEME_EXTR_MAXCHARS starting at byte position 6 = a%CC%8Axyz == a%CC%8Axyz
