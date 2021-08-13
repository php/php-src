--TEST--
sort_with_sort_keys()
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '51.2') >=  0) die('skip for ICU < 51.2'); ?>
--FILE--
<?php

/*
 * Sort arrays using various locales.
 */


$test_num = 1;

/*
 * Sort arrays in the given list using specified locale.
 */
function sort_arrays( $locale, $arrays )
{
    $res_str = '';

    $coll = ut_coll_create( $locale );

    foreach( $arrays as $array )
    {
        // Sort array values
        $res_val = ut_coll_sort_with_sort_keys( $coll, $array );

        // Concatenate the sorted array and function result
        // with output string.
        $res_dump = "\n" . dump( $array ) .
                    "\n Result: " . dump( $res_val );


        // Preppend test signature to output string
        $md5 = md5( $res_dump );

        global $test_num;

        $res_str .= "\n\n".
                    "Test $test_num.$md5:" .
                    $res_dump;
        ++$test_num;
    }

    return $res_str;
}


function ut_main()
{
    global $test_num;
    $test_num = 1;
    $res_str = '';

    // Sort an array in SORT_REGULAR mode using en_US locale.
    $test_params = array(
        array( 'abc', 'abd', 'aaa' ),
        array( 'm'  , '1'  , '_'   ),
        array( 'a'  , 'aaa', 'aa'  ),
        array( 'ba' , 'b'  , 'ab'  ),
        array( 'e'  , 'c'  , 'a'   ),
        array( 'd'  , ''   , ' a'  ),
        array( 'd ' , 'f ' , ' a'  ),
        array( 'a'  , null , '3'   ),
        array( 'y'  , 'i'  , 'k'   )
    );

    $res_str .= sort_arrays( 'en_US', $test_params );

    // Sort a non-ASCII array using ru_RU locale.
    $test_params = array(
        array( 'абг', 'абв', 'ааа', 'abc' ),
        array( 'аа', 'ааа', 'а' )
    );

    $res_str .= sort_arrays( 'ru_RU', $test_params );

    // Array with data for sorting.
    $test_params = array(
        array( 'y'  , 'i'  , 'k'   )
    );

    // Sort an array using Lithuanian locale.
    $res_str .= sort_arrays( 'lt_LT', $test_params );

    return $res_str . "\n";
}

include_once( 'ut_common.inc' );
ut_run();
?>
--EXPECT--
Test 1.e8f1cd28133d79ecd660002f1c660d0e:
array (
  0 => 'aaa',
  1 => 'abc',
  2 => 'abd',
)
 Result: true

Test 2.c2ded12173dd2996927378cae37eb275:
array (
  0 => '_',
  1 => '1',
  2 => 'm',
)
 Result: true

Test 3.54071c968d71cb98c5d379145f8d7d38:
array (
  0 => 'a',
  1 => 'aa',
  2 => 'aaa',
)
 Result: true

Test 4.19abe63d6f6dfef65b0e3c9ab4826b07:
array (
  0 => 'ab',
  1 => 'b',
  2 => 'ba',
)
 Result: true

Test 5.9a8dc0a9bc771368c2f1fc3d02754610:
array (
  0 => 'a',
  1 => 'c',
  2 => 'e',
)
 Result: true

Test 6.923d65739c5219c634616ffd100a50e4:
array (
  0 => '',
  1 => ' a',
  2 => 'd',
)
 Result: true

Test 7.289bc2f28e87d3201ec9d7e8477ae1b0:
array (
  0 => ' a',
  1 => 'd ',
  2 => 'f ',
)
 Result: true

Test 8.de0fd958484f2377a645835d7fbcf124:
array (
  0 => NULL,
  1 => '3',
  2 => 'a',
)
 Result: true

Test 9.dd2b8f0adb37c45d528cad1a0cc0f361:
array (
  0 => 'i',
  1 => 'k',
  2 => 'y',
)
 Result: true

Test 10.ca0e38a2e3147dd97070f2128f140934:
array (
  0 => 'abc',
  1 => 'ааа',
  2 => 'абв',
  3 => 'абг',
)
 Result: true

Test 11.91480b10473a0c96a4cd6d88c23c577a:
array (
  0 => 'а',
  1 => 'аа',
  2 => 'ааа',
)
 Result: true

Test 12.fdd3fe3981476039164aa000bf9177f2:
array (
  0 => 'i',
  1 => 'y',
  2 => 'k',
)
 Result: true
