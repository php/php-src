--TEST--
mb_split()
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_split') or die("skip mb_split() is not available in this build");
?>
--FILE--
<?php
    mb_regex_set_options( '' );
    mb_regex_encoding( 'EUC-JP' );

    function verify_split( $spliton, $str, $count = 0 )
    {
        $result1 = mb_split( $spliton, $str, $count );
        $result2 = preg_split( "/$spliton/", $str, $count );
        if ( $result1 == $result2 ) {
            print "ok\n";
        } else {
            print count($result1).'-'.count($result2)."\n";
        }
    }

    var_dump( mb_split( " ", "a b c d e f g" )
              == mb_split( "[[:space:]]", "a\nb\tc\nd e f g" ) );

    for ( $i = 1; $i < 5; ++$i ) {
        verify_split( " ", "a\tb\tc\td   e\tf g", $i );
    }

    for ( $i = 1; $i < 5; ++$i ) {
        verify_split( "\xa1\xa1+", "\xa1\xa1\xa1\xa2\xa2\xa1\xa1\xa1\xa1\xa1\xa1\xa2\xa2\xa1\xa1\xa1", $i );
    }
?>
--EXPECTF--
Deprecated: Function mb_regex_set_options() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_split() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_split() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_split() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
ok

Deprecated: Function mb_split() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
ok

Deprecated: Function mb_split() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
ok

Deprecated: Function mb_split() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
ok

Deprecated: Function mb_split() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
ok

Deprecated: Function mb_split() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
2-2

Deprecated: Function mb_split() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
3-3

Deprecated: Function mb_split() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
4-4
