--TEST--
mb_strtoupper() / mb_strtolower()
--EXTENSIONS--
mbstring
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
--EXPECTF--
Deprecated: Function mb_internal_encoding() is deprecated since 8.5, use internal_encoding INI settings instead in %s on line %d
abcdefghijklmnopqrstuvwxyz
ABCDEFGHIJKLMNOPQRSTUVWXYZ
ִֻֿ
ִכן
