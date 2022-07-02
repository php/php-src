--TEST--
collator_get_sort_key() icu >= 54.1
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '55.1') >=  0) die('skip for ICU < 55.1'); ?>
<?php if (version_compare(INTL_ICU_VERSION, '54.1') < 0) die('skip for ICU >= 54.1'); ?>
--FILE--
<?php

/*
 * Get sort keys using various locales
 */
function sort_arrays( $locale, $data )
{
    $res_str = '';

    $coll = ut_coll_create( $locale );

    foreach($data as $value) {
        $res_val = ut_coll_get_sort_key( $coll, $value );
        $res_str .= "source: ".$value."\n".
                    "key: ".bin2hex($res_val)."\n";
    }

    return $res_str;
}


function ut_main()
{
    $res_str = '';

    // Regular strings keys
    $test_params = array(
        'abc', 'abd', 'aaa',
        'аа', 'а', 'z',
        '', '3',
        'y'  , 'i'  , 'k'
    );

    $res_str .= sort_arrays( 'en_US', $test_params );

    // Sort a non-ASCII array using ru_RU locale.
    $test_params = array(
        'абг', 'абв', 'жжж', 'эюя'
    );

    $res_str .= sort_arrays( 'ru_RU', $test_params );

    // Sort an array using Lithuanian locale.
    $res_str .= sort_arrays( 'lt_LT', $test_params );

    return $res_str . "\n";
}

include_once( 'ut_common.inc' );
ut_run();
?>
--EXPECT--
source: abc
key: 292b2d01070107
source: abd
key: 292b2f01070107
source: aaa
key: 29292901070107
source: аа
key: 5e060601060106
source: а
key: 5e0601050105
source: z
key: 5b01050105
source: 
key: 0101
source: 3
key: 1a01050105
source: y
key: 5901050105
source: i
key: 3901050105
source: k
key: 3d01050105
source: абг
key: 2806101401070107
source: абв
key: 2806101201070107
source: жжж
key: 2830303001070107
source: эюя
key: 28ccd0d401070107
source: абг
key: 5e06101401070107
source: абв
key: 5e06101201070107
source: жжж
key: 5e30303001070107
source: эюя
key: 5eccd0d401070107
