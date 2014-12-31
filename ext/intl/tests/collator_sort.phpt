--TEST--
sort()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
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
function sort_arrays( $locale, $arrays, $sort_flag = Collator::SORT_REGULAR )
{
    $res_str = '';

    $coll = ut_coll_create( $locale );

    foreach( $arrays as $array )
    {
        // Sort array values
        $res_val = ut_coll_sort( $coll, $array, $sort_flag );

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
        array( '100', '25' , '36'  ),
        array( 5    , '30' , 2     ),
        array( 'd'  , ''   , ' a'  ),
        array( 'd ' , 'f ' , ' a'  ),
        array( 'a'  , null , '3'   ),
        array( 'y'  , 'k'  , 'i' )
    );

    $res_str .= sort_arrays( 'en_US', $test_params );

    $test_params = array(
        array( '100', '25' , '36'  ),
        array( 5    , '30' , 2     ),
        array( 'd'  , ''   , ' a'  ),
        array( 'y'  , 'k'  , 'i' )
    );

    // Sort in en_US locale with SORT_STRING flag
    $res_str .= sort_arrays( 'en_US', $test_params, Collator::SORT_STRING );


    // Sort a non-ASCII array using ru_RU locale.
    $test_params = array(
        array( 'абг', 'абв', 'ааа', 'abc' ),
        array( 'аа', 'ааа' , 'а' )
    );

    $res_str .= sort_arrays( 'ru_RU', $test_params );

    // Sort an array using Lithuanian locale.
    $test_params = array(
        array( 'y'  , 'k'  , 'i' )
    );

    $res_str .= sort_arrays( 'lt_LT', $test_params );

    return $res_str;
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

Test 6.ab530b060e5e54a65bfb8b9f8fc61870:
array (
  0 => '25',
  1 => '36',
  2 => '100',
)
 Result: true

Test 7.0718dd838509017bded2ed307a6e785f:
array (
  0 => 2,
  1 => 5,
  2 => '30',
)
 Result: true

Test 8.923d65739c5219c634616ffd100a50e4:
array (
  0 => '',
  1 => ' a',
  2 => 'd',
)
 Result: true

Test 9.289bc2f28e87d3201ec9d7e8477ae1b0:
array (
  0 => ' a',
  1 => 'd ',
  2 => 'f ',
)
 Result: true

Test 10.de0fd958484f2377a645835d7fbcf124:
array (
  0 => NULL,
  1 => '3',
  2 => 'a',
)
 Result: true

Test 11.dd2b8f0adb37c45d528cad1a0cc0f361:
array (
  0 => 'i',
  1 => 'k',
  2 => 'y',
)
 Result: true

Test 12.1e6b4d6f7df9d4580317634ea46d8208:
array (
  0 => '100',
  1 => '25',
  2 => '36',
)
 Result: true

Test 13.cec115dc9850b98dfbdf102efa09e61b:
array (
  0 => 2,
  1 => '30',
  2 => 5,
)
 Result: true

Test 14.923d65739c5219c634616ffd100a50e4:
array (
  0 => '',
  1 => ' a',
  2 => 'd',
)
 Result: true

Test 15.dd2b8f0adb37c45d528cad1a0cc0f361:
array (
  0 => 'i',
  1 => 'k',
  2 => 'y',
)
 Result: true

Test 16.ca0e38a2e3147dd97070f2128f140934:
array (
  0 => 'abc',
  1 => 'ааа',
  2 => 'абв',
  3 => 'абг',
)
 Result: true

Test 17.91480b10473a0c96a4cd6d88c23c577a:
array (
  0 => 'а',
  1 => 'аа',
  2 => 'ааа',
)
 Result: true

Test 18.fdd3fe3981476039164aa000bf9177f2:
array (
  0 => 'i',
  1 => 'y',
  2 => 'k',
)
 Result: true
