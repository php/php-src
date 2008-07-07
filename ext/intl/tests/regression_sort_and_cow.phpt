--TEST--
Regression: sort() and copy-on-write.
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
/*
 * Check if collator_sort() properly supports copy-on-write.
 */


/* Create two copies of the given array.
 * Sort the array and the first copy.
 * Check if the second copy remains unsorted.
 */
function test_COW( $locale, $test_array )
{
    $res_str = '';

    $coll = ut_coll_create( $locale );

    // Convert strings to UTF-16 if needed.
    $u_test_array = u( $test_array );

    // Create two copies of the given array.
    $u_copy1 = $u_test_array;
    $u_copy2 = $u_test_array;

    // Sort given array and the first copy of it.
    ut_coll_sort( $coll, $u_test_array );
    ut_coll_sort( $coll, $u_copy1      );

    // Return contents of all the arrays.
    // The second copy should remain unsorted.
    $res_str .= dump_array( $u_test_array ) . "\n";
    $res_str .= dump_array( $u_copy1 ) . "\n";
    $res_str .= dump_array( $u_copy2 ) . "\n";

    return $res_str;
}

function ut_main()
{
    $res_str = '';

    $a1 = array( 'b', 'a', 'c' );
    $a2 = array( 'в', 'а', 'б' );

    $res_str .= test_COW( 'en_US', $a1 );
    $res_str .= test_COW( 'ru_RU', $a2 );

    return $res_str;
}

require_once( 'ut_common.inc' );
ut_run();
?>
--EXPECT--
array (
  0 => 'a',
  1 => 'b',
  2 => 'c',
)
array (
  0 => 'a',
  1 => 'b',
  2 => 'c',
)
array (
  0 => 'b',
  1 => 'a',
  2 => 'c',
)
array (
  0 => 'а',
  1 => 'б',
  2 => 'в',
)
array (
  0 => 'а',
  1 => 'б',
  2 => 'в',
)
array (
  0 => 'в',
  1 => 'а',
  2 => 'б',
)
