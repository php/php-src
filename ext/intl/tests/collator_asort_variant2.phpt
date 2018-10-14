--TEST--
asort()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
<?php if (version_compare(INTL_ICU_VERSION, '51.2') < 0) die('skip for ICU >= 51.2'); ?>
--FILE--
<?php

/*
 * Sort associative arrays using various locales.
 */


$test_num = 1;

/*
 * Sort various arrays in specified locale.
 */
function sort_arrays( $locale, $test_arrays, $sort_flag = Collator::SORT_REGULAR )
{
    $res_str = '';

    $coll = ut_coll_create( $locale );

    foreach( $test_arrays as $test_array )
    {
        // Try to sort test data.
        $res_val = ut_coll_asort( $coll, $test_array, $sort_flag );

        // Return output data.
        $res_dump = "\n" . dump( $test_array ) .
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

/*
 * Test main function.
 */
function ut_main()
{
    global $test_num;
    $test_num = 1;
    $res_str = '';

    // Sort an array in SORT_REGULAR mode using en_US locale.
    $test_params = array(
        array( 'd' => 'y'  ,
               'c' => 'i'  ,
               'a' => 'k'  ),

        array( 'a' => 'a'  ,
               'b' => 'aaa',
               'c' => 'aa' ),

        array( 'a'  => 'a' ,
               'aaa'=> 'a' ,
               'aa' => 'a' ),

        array( '1' => 'abc',
               '5' => '!'  ,
               '2' => null ,
               '7' => ''   ),

        array( '1' => '100',
               '2' => '25' ,
               '3' => '36' ),

        array( '1' => 5    ,
               '2' => '30' ,
               '3' => 2    )
    );

    $res_str .= sort_arrays( 'en_US', $test_params );

    // Sort an array in SORT_STRING mode using en_US locale.
    $test_params = array(
        array( '1' => '100',
               '2' => '25' ,
               '3' => '36' ),

        array( '1' => 5    ,
               '2' => '30' ,
               '3' => 2    ),

        array( '1' => 'd'  ,
               '2' => ''   ,
               '3' => ' a' ),

        array( '1' => 'y'  ,
               '2' => 'k'  ,
               '3' => 'i'  )
    );

    $res_str .= sort_arrays( 'en_US', $test_params, Collator::SORT_STRING );

    // Sort a non-ASCII array using ru_RU locale.
    $test_params = array(
        array( 'п' => 'у',
               'б' => 'в',
               'е' => 'а' ),

        array( '1' => 'п',
               '4' => '',
               '7' => 'd',
               '2' => 'пп' )
    );

    $res_str .= sort_arrays( 'ru_RU', $test_params );


    // Sort an array using Lithuanian locale.
    $test_params = array(
        array( 'd' => 'y',
               'c' => 'i',
               'a' => 'k' )
    );

    $res_str .= sort_arrays( 'lt_LT', $test_params );

    return $res_str . "\n";
}

include_once( 'ut_common.inc' );
ut_run();
?>
--EXPECT--
Test 1.162b81ac12878b817fc39063097e45b5:
array (
  'c' => 'i',
  'a' => 'k',
  'd' => 'y',
)
 Result: true

Test 2.93d96e22f692d8a281b0a389f01f8d1e:
array (
  'a' => 'a',
  'c' => 'aa',
  'b' => 'aaa',
)
 Result: true

Test 3.d5a9747a8bd4c32cb2a705f7e6d8a56f:
array (
  'a' => 'a',
  'aaa' => 'a',
  'aa' => 'a',
)
 Result: true

Test 4.dcc371022fa1eac76e73b0455d70790a:
array (
  2 => NULL,
  7 => '',
  5 => '!',
  1 => 'abc',
)
 Result: true

Test 5.99dc71f405b286e03d489061b36e6900:
array (
  2 => '25',
  3 => '36',
  1 => '100',
)
 Result: true

Test 6.bf5bba243307c9d12934e756ad4be190:
array (
  3 => 2,
  1 => 5,
  2 => '30',
)
 Result: true

Test 7.e4ee7024c61476e9e7a6c28b5e47df6f:
array (
  1 => '100',
  2 => '25',
  3 => '36',
)
 Result: true

Test 8.5fa7033dd43784be0db1474eb48b83c8:
array (
  3 => 2,
  2 => '30',
  1 => 5,
)
 Result: true

Test 9.588cdf4692bc09aa92ffe7e48f9e4579:
array (
  2 => '',
  3 => ' a',
  1 => 'd',
)
 Result: true

Test 10.be02641a47ebcccd23e4183ca3a415f7:
array (
  3 => 'i',
  2 => 'k',
  1 => 'y',
)
 Result: true

Test 11.153d9b11d1e5936afc917a94a4e11f34:
array (
  'е' => 'а',
  'б' => 'в',
  'п' => 'у',
)
 Result: true

Test 12.e1f5cb037b564dce39ffbd0a61562d59:
array (
  4 => '',
  1 => 'п',
  2 => 'пп',
  7 => 'd',
)
 Result: true

Test 13.8800d48abb960a59002eef77f1d73ae0:
array (
  'c' => 'i',
  'd' => 'y',
  'a' => 'k',
)
 Result: true
