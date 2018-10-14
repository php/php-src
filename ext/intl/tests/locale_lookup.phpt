--TEST--
locale_lookup.phpt() icu >= 4.8 && icu < 50.1.2
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
<?php if (version_compare(INTL_ICU_VERSION, '50.1.2') >=  0) die('skip for ICU < 50.1.2'); ?>
--FILE--
<?php

/*
 * Try parsing different Locales
 * with Procedural and Object methods.
 */

function ut_main()
{
	$loc_ranges = array(
		'de-de',
		'sl_IT',
		'sl_IT_Nedis',
		'jbo',
		'art-lojban'
	);

	$lang_tags = array(
		'de-DEVA',
		'de-DE-1996',
		'de-DE',
		'zh_Hans',
		'de-CH-1996',
		'sl_IT',
		'sl_IT_nedis-a-kirti-x-xyz',
		'sl_IT_rozaj',
		'sl_IT_NEDIS_ROJAZ_1901',
		'i-enochian',
		'sgn-CH-de',
		'art-lojban',
		'i-lux',
		'art-lojban',
		'jbo',
		'en_sl_IT',
		'zh-Hant-CN-x-prv1-prv2'
	);


    $res_str = '';
    $isCanonical = false;

	foreach($loc_ranges as $loc_range){
            $res_str .="--------------\n";
            $result= ut_loc_locale_lookup( $lang_tags , $loc_range,$isCanonical,"en_US");
            $comma_arr =implode(",",$lang_tags);
            $res_str .= "loc_range:$loc_range \nlang_tags: $comma_arr\n";
            $res_str .= "\nlookup result:$result\n";
//canonicalized version
            $result= ut_loc_locale_lookup( $lang_tags , $loc_range,!($isCanonical),"en_US");
            $can_loc_range = ut_loc_canonicalize($loc_range);
            $res_str .= "Canonical lookup result:$result\n";

	}

	$res_str .= "\n";
    return $res_str;

}

include_once( 'ut_common.inc' );
ut_run();

?>
--EXPECT--
--------------
loc_range:de-de 
lang_tags: de-DEVA,de-DE-1996,de-DE,zh_Hans,de-CH-1996,sl_IT,sl_IT_nedis-a-kirti-x-xyz,sl_IT_rozaj,sl_IT_NEDIS_ROJAZ_1901,i-enochian,sgn-CH-de,art-lojban,i-lux,art-lojban,jbo,en_sl_IT,zh-Hant-CN-x-prv1-prv2

lookup result:de-DE
Canonical lookup result:de_de
--------------
loc_range:sl_IT 
lang_tags: de-DEVA,de-DE-1996,de-DE,zh_Hans,de-CH-1996,sl_IT,sl_IT_nedis-a-kirti-x-xyz,sl_IT_rozaj,sl_IT_NEDIS_ROJAZ_1901,i-enochian,sgn-CH-de,art-lojban,i-lux,art-lojban,jbo,en_sl_IT,zh-Hant-CN-x-prv1-prv2

lookup result:sl_IT
Canonical lookup result:sl_it
--------------
loc_range:sl_IT_Nedis 
lang_tags: de-DEVA,de-DE-1996,de-DE,zh_Hans,de-CH-1996,sl_IT,sl_IT_nedis-a-kirti-x-xyz,sl_IT_rozaj,sl_IT_NEDIS_ROJAZ_1901,i-enochian,sgn-CH-de,art-lojban,i-lux,art-lojban,jbo,en_sl_IT,zh-Hant-CN-x-prv1-prv2

lookup result:sl_IT
Canonical lookup result:sl_it
--------------
loc_range:jbo 
lang_tags: de-DEVA,de-DE-1996,de-DE,zh_Hans,de-CH-1996,sl_IT,sl_IT_nedis-a-kirti-x-xyz,sl_IT_rozaj,sl_IT_NEDIS_ROJAZ_1901,i-enochian,sgn-CH-de,art-lojban,i-lux,art-lojban,jbo,en_sl_IT,zh-Hant-CN-x-prv1-prv2

lookup result:jbo
Canonical lookup result:jbo
--------------
loc_range:art-lojban 
lang_tags: de-DEVA,de-DE-1996,de-DE,zh_Hans,de-CH-1996,sl_IT,sl_IT_nedis-a-kirti-x-xyz,sl_IT_rozaj,sl_IT_NEDIS_ROJAZ_1901,i-enochian,sgn-CH-de,art-lojban,i-lux,art-lojban,jbo,en_sl_IT,zh-Hant-CN-x-prv1-prv2

lookup result:art-lojban
Canonical lookup result:jbo
