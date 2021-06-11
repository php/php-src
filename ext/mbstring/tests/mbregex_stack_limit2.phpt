--TEST--
Test oniguruma stack limit
--EXTENSIONS--
mbstring
iconv
--SKIPIF--
<?php
if (!function_exists('mb_ereg_replace')) die('skip mb_ereg_replace not available');
if (version_compare(MB_ONIGURUMA_VERSION, '6.9.3') < 0) {
    die('skip requires Oniguruma 6.9.3');
}
?>
--FILE--
<?php
function mb_trim( $string, $chars = "", $chars_array = array() )
{
    for( $x=0; $x<iconv_strlen( $chars ); $x++ ) $chars_array[] = preg_quote( iconv_substr( $chars, $x, 1 ) );
    $encoded_char_list = implode( "|", array_merge( array( "\s","\t","\n","\r", "\0", "\x0B" ), $chars_array ) );

    $string = mb_ereg_replace( "^($encoded_char_list)*", "", $string );
    $string = mb_ereg_replace( "($encoded_char_list)*$", "", $string );
    return $string;
}

ini_set('mbstring.regex_stack_limit', 10000);
var_dump(mb_trim(str_repeat(' ', 10000)));

echo 'OK';
?>
--EXPECTF--
Warning: mb_ereg_replace(): mbregex search failure in php_mbereg_replace_exec(): match-stack limit over in %s on line %d
string(0) ""
OK
