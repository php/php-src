--TEST--
collator_get_sort_key()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--XFAIL--
Sort keys are not fixed, comparing them to fixed strings doesn't work.
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
		$res_str .= "source: ".urlencode($value)."\n".
					"key: ".urlencode($res_val)."\n";
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
		'', null , '3',
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
key: %29%2B-%01%07%01%07%00
source: abd
key: %29%2B%2F%01%07%01%07%00
source: aaa
key: %29%29%29%01%07%01%07%00
source: %D0%B0%D0%B0
key: _++%01%06%01%06%00
source: %D0%B0
key: _+%01%05%01%05%00
source: z
key: %5B%01%05%01%05%00
source:
key: %01%01%00
source:
key: %01%01%00
source: 3
key: %26%80%01%05%01%05%00
source: y
key: Y%01%05%01%05%00
source: i
key: 9%01%05%01%05%00
source: k
key: %3D%01%05%01%05%00
source: %D0%B0%D0%B1%D0%B3
key: _+%2C0%01%07%01%07%00
source: %D0%B0%D0%B1%D0%B2
key: _+%2C.%01%07%01%07%00
source: %D0%B6%D0%B6%D0%B6
key: _LLL%01%07%01%07%00
source: %D1%8D%D1%8E%D1%8F
key: %60%05%09%0B%01%07%01%07%00
source: %D0%B0%D0%B1%D0%B3
key: _+%2C0%01%07%01%07%00
source: %D0%B0%D0%B1%D0%B2
key: _+%2C.%01%07%01%07%00
source: %D0%B6%D0%B6%D0%B6
key: _LLL%01%07%01%07%00
source: %D1%8D%D1%8E%D1%8F
key: %60%05%09%0B%01%07%01%07%00