--TEST--
compare()
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '51.2') < 0) die('skip for ICU >= 51.2'); ?>
--FILE--
<?php

/*
 * Compare various string pairs using various locales.
 */


/*
 * Converts comparison result to a character.
 */
function cmp_to_char( $comp_res )
{
    switch( $comp_res )
    {
    case 0:            // UCOL_EQUAL
        return '=';
    case 1:            // UCOL_GREATER
        return '>';
    case -1:           // UCOL_LESS
        return '<';
    default:
        return '?';
    }
}

/*
 * Compare string pairs in the given array
 * using specified locale.
 */
function compare_pairs( $locale, $test_array )
{
    $res_str = '';

    $coll = ut_coll_create( $locale );

    foreach( $test_array as $test_strings )
    {
        list( $str1, $str2 ) = $test_strings;

        // Compare strings.
        $res_val = cmp_to_char( ut_coll_compare( $coll, $str1, $str2 ) );

        // Concatenate result strings.
        $res_str .= dump( $str1 ) .
                    ' ' . $res_val . ' ' .
                    dump( $str2 ) . "\n";
    }

    return $res_str;

}

function ut_main()
{
    $res_str = '';

    // Compare strings using en_US locale.
    $test_params = array(
        array( 'abc', 'abc' ),
        array( 'Abc', 'abc' ),
        array( 'a'  , 'abc' ),
        array( 'a'  , ''    ),
        array( ''  , ''     ),
        array( 'a'  , 'b'   ),
        array( 'ab'  , 'b'  ),
        array( 'ab'  , 'a'  ),
        array( 123  , 'abc' ),
        array( 'ac' , ''    ),
        array( '.'  , '.'   ),
        // Try to compare long strings.
        array( 'abcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcde',
               'abcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdea'),
    );

    $res_str .= compare_pairs( 'en_US', $test_params );


    // Compare strings using ru_RU locale.
    $test_params = array(
        array( 'а',   'б' ),
        array( 'а',   'аа' ),
        array( 'аб', 'ба' ),
        array( 'а',   ',' ),
        array( 'а',   'b' ),
        array( 'а',   'bb' ),
        array( 'а',   'ab' ),
        array( 'а',   '' )
    );

    $res_str .= compare_pairs( 'ru_RU', $test_params );


    // Compare strings using lt_LT locale.
    $test_params = array(
        array( 'y', 'k' )
    );

    $res_str .= compare_pairs( 'lt_LT', $test_params );

    return $res_str;
}

include_once( 'ut_common.inc' );
ut_run();
?>
--EXPECT--
'abc' = 'abc'
'Abc' > 'abc'
'a' < 'abc'
'a' > ''
'' = ''
'a' < 'b'
'ab' < 'b'
'ab' > 'a'
123 < 'abc'
'ac' > ''
'.' = '.'
'abcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcde' < 'abcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdea'
'а' < 'б'
'а' < 'аа'
'аб' < 'ба'
'а' > ','
'а' < 'b'
'а' < 'bb'
'а' < 'ab'
'а' > ''
'y' < 'k'
