--TEST--
Regression: sort_wsk() eq but different len.
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
/*
 * Test sorting strings that have different length but otherwise equal.
 */

function sort_using_locale( $locale, $test_array )
{
    $coll = ut_coll_create( $locale );

    // Sort array.
    ut_coll_sort_with_sort_keys( $coll, $test_array );

    // And return the sorted array.
    return dump( $test_array ) . "\n";
}

function ut_main()
{
    $res_str = '';

    // Define a couple of arrays.
    // Each array contains equal strings that differ only in their length.
    $a1 = array( 'aa', 'aaa', 'a' );
    $a2 = array( 'пп', 'ппп', 'п' );

    // Sort them.
    $res_str .= sort_using_locale( 'en_US', $a1 );
    $res_str .= sort_using_locale( 'ru_RU', $a2 );

    return $res_str;
}

require_once( 'ut_common.inc' );
ut_run();
?>
--EXPECT--
array (
  0 => 'a',
  1 => 'aa',
  2 => 'aaa',
)
array (
  0 => 'п',
  1 => 'пп',
  2 => 'ппп',
)
