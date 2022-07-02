--TEST--
collator_get_sort_key()
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '51.2') < 0) die('skip for ICU >= 51.2'); ?>
<?php if (version_compare(INTL_ICU_VERSION, '53.1') >=  0) die('skip for ICU < 53.1'); ?>
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
key: 27292b01070107
source: abd
key: 27292d01070107
source: aaa
key: 27272701070107
source: аа
key: 5c0a0a01060106
source: а
key: 5c0a01050105
source: z
key: 5901050105
source: 
key: 0101
source: 3
key: 1801050105
source: y
key: 5701050105
source: i
key: 3701050105
source: k
key: 3b01050105
source: абг
key: 260a161a01070107
source: абв
key: 260a161801070107
source: жжж
key: 263a3a3a01070107
source: эюя
key: 273b3f4501070107
source: абг
key: 5c0a161a01070107
source: абв
key: 5c0a161801070107
source: жжж
key: 5c3a3a3a01070107
source: эюя
key: 5d3b3f4501070107
