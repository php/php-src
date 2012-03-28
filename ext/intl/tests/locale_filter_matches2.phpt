--TEST--
locale_filter_matches.phpt() icu >= 4.8
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
<?php if(version_compare(INTL_ICU_VERSION, '4.8') < 0) print 'skip'; ?>
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
		'art-lojban',
		'sl_IT'
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
		'en_sl_IT'
	);

    $res_str = '';
    $isCanonical = false;
	foreach($loc_ranges as $loc_range){
		foreach($lang_tags as $lang_tag){
			$res_str .="--------------\n";
			$result= ut_loc_locale_filter_matches( $lang_tag , $loc_range , $isCanonical);
			$res_str .= "loc_range:$loc_range matches lang_tag $lang_tag ? ";
			if( $result){	
				$res_str .= "YES\n";
			}else{
				$res_str .= "NO\n";
			}
//canonicalized version
			$result= ut_loc_locale_filter_matches( $lang_tag , $loc_range , !($isCanonical));
			$can_loc_range = ut_loc_canonicalize($loc_range);
			$can_lang_tag = ut_loc_canonicalize($lang_tag);
			$res_str .= "loc_range:$can_loc_range canonically matches lang_tag $can_lang_tag ? ";
			if( $result){	
				$res_str .= "YES\n";
			}else{
				$res_str .= "NO\n";
			}
		}
	}

	$res_str .= "\n";
    return $res_str;

}

include_once( 'ut_common.inc' );
ut_run();

?>
--EXPECT--
--------------
loc_range:de-de matches lang_tag de-DEVA ? NO
loc_range:de_DE canonically matches lang_tag de_Deva ? NO
--------------
loc_range:de-de matches lang_tag de-DE-1996 ? YES
loc_range:de_DE canonically matches lang_tag de_DE_1996 ? YES
--------------
loc_range:de-de matches lang_tag de-DE ? YES
loc_range:de_DE canonically matches lang_tag de_DE ? YES
--------------
loc_range:de-de matches lang_tag zh_Hans ? NO
loc_range:de_DE canonically matches lang_tag zh_Hans ? NO
--------------
loc_range:de-de matches lang_tag de-CH-1996 ? NO
loc_range:de_DE canonically matches lang_tag de_CH_1996 ? NO
--------------
loc_range:de-de matches lang_tag sl_IT ? NO
loc_range:de_DE canonically matches lang_tag sl_IT ? NO
--------------
loc_range:de-de matches lang_tag sl_IT_nedis-a-kirti-x-xyz ? NO
loc_range:de_DE canonically matches lang_tag sl_IT_NEDIS_A_KIRTI_X_XYZ ? NO
--------------
loc_range:de-de matches lang_tag sl_IT_rozaj ? NO
loc_range:de_DE canonically matches lang_tag sl_IT_ROZAJ ? NO
--------------
loc_range:de-de matches lang_tag sl_IT_NEDIS_ROJAZ_1901 ? NO
loc_range:de_DE canonically matches lang_tag sl_IT_NEDIS_ROJAZ_1901 ? NO
--------------
loc_range:de-de matches lang_tag i-enochian ? NO
loc_range:de_DE canonically matches lang_tag @x=i-enochian ? NO
--------------
loc_range:de-de matches lang_tag sgn-CH-de ? NO
loc_range:de_DE canonically matches lang_tag sgn_CH_DE ? NO
--------------
loc_range:de-de matches lang_tag art-lojban ? NO
loc_range:de_DE canonically matches lang_tag jbo ? NO
--------------
loc_range:de-de matches lang_tag i-lux ? NO
loc_range:de_DE canonically matches lang_tag lb ? NO
--------------
loc_range:de-de matches lang_tag art-lojban ? NO
loc_range:de_DE canonically matches lang_tag jbo ? NO
--------------
loc_range:de-de matches lang_tag jbo ? NO
loc_range:de_DE canonically matches lang_tag jbo ? NO
--------------
loc_range:de-de matches lang_tag en_sl_IT ? NO
loc_range:de_DE canonically matches lang_tag en_SL_IT ? NO
--------------
loc_range:sl_IT matches lang_tag de-DEVA ? NO
loc_range:sl_IT canonically matches lang_tag de_Deva ? NO
--------------
loc_range:sl_IT matches lang_tag de-DE-1996 ? NO
loc_range:sl_IT canonically matches lang_tag de_DE_1996 ? NO
--------------
loc_range:sl_IT matches lang_tag de-DE ? NO
loc_range:sl_IT canonically matches lang_tag de_DE ? NO
--------------
loc_range:sl_IT matches lang_tag zh_Hans ? NO
loc_range:sl_IT canonically matches lang_tag zh_Hans ? NO
--------------
loc_range:sl_IT matches lang_tag de-CH-1996 ? NO
loc_range:sl_IT canonically matches lang_tag de_CH_1996 ? NO
--------------
loc_range:sl_IT matches lang_tag sl_IT ? YES
loc_range:sl_IT canonically matches lang_tag sl_IT ? YES
--------------
loc_range:sl_IT matches lang_tag sl_IT_nedis-a-kirti-x-xyz ? YES
loc_range:sl_IT canonically matches lang_tag sl_IT_NEDIS_A_KIRTI_X_XYZ ? YES
--------------
loc_range:sl_IT matches lang_tag sl_IT_rozaj ? YES
loc_range:sl_IT canonically matches lang_tag sl_IT_ROZAJ ? YES
--------------
loc_range:sl_IT matches lang_tag sl_IT_NEDIS_ROJAZ_1901 ? YES
loc_range:sl_IT canonically matches lang_tag sl_IT_NEDIS_ROJAZ_1901 ? YES
--------------
loc_range:sl_IT matches lang_tag i-enochian ? NO
loc_range:sl_IT canonically matches lang_tag @x=i-enochian ? NO
--------------
loc_range:sl_IT matches lang_tag sgn-CH-de ? NO
loc_range:sl_IT canonically matches lang_tag sgn_CH_DE ? NO
--------------
loc_range:sl_IT matches lang_tag art-lojban ? NO
loc_range:sl_IT canonically matches lang_tag jbo ? NO
--------------
loc_range:sl_IT matches lang_tag i-lux ? NO
loc_range:sl_IT canonically matches lang_tag lb ? NO
--------------
loc_range:sl_IT matches lang_tag art-lojban ? NO
loc_range:sl_IT canonically matches lang_tag jbo ? NO
--------------
loc_range:sl_IT matches lang_tag jbo ? NO
loc_range:sl_IT canonically matches lang_tag jbo ? NO
--------------
loc_range:sl_IT matches lang_tag en_sl_IT ? NO
loc_range:sl_IT canonically matches lang_tag en_SL_IT ? NO
--------------
loc_range:sl_IT_Nedis matches lang_tag de-DEVA ? NO
loc_range:sl_IT_NEDIS canonically matches lang_tag de_Deva ? NO
--------------
loc_range:sl_IT_Nedis matches lang_tag de-DE-1996 ? NO
loc_range:sl_IT_NEDIS canonically matches lang_tag de_DE_1996 ? NO
--------------
loc_range:sl_IT_Nedis matches lang_tag de-DE ? NO
loc_range:sl_IT_NEDIS canonically matches lang_tag de_DE ? NO
--------------
loc_range:sl_IT_Nedis matches lang_tag zh_Hans ? NO
loc_range:sl_IT_NEDIS canonically matches lang_tag zh_Hans ? NO
--------------
loc_range:sl_IT_Nedis matches lang_tag de-CH-1996 ? NO
loc_range:sl_IT_NEDIS canonically matches lang_tag de_CH_1996 ? NO
--------------
loc_range:sl_IT_Nedis matches lang_tag sl_IT ? NO
loc_range:sl_IT_NEDIS canonically matches lang_tag sl_IT ? NO
--------------
loc_range:sl_IT_Nedis matches lang_tag sl_IT_nedis-a-kirti-x-xyz ? YES
loc_range:sl_IT_NEDIS canonically matches lang_tag sl_IT_NEDIS_A_KIRTI_X_XYZ ? YES
--------------
loc_range:sl_IT_Nedis matches lang_tag sl_IT_rozaj ? NO
loc_range:sl_IT_NEDIS canonically matches lang_tag sl_IT_ROZAJ ? NO
--------------
loc_range:sl_IT_Nedis matches lang_tag sl_IT_NEDIS_ROJAZ_1901 ? YES
loc_range:sl_IT_NEDIS canonically matches lang_tag sl_IT_NEDIS_ROJAZ_1901 ? YES
--------------
loc_range:sl_IT_Nedis matches lang_tag i-enochian ? NO
loc_range:sl_IT_NEDIS canonically matches lang_tag @x=i-enochian ? NO
--------------
loc_range:sl_IT_Nedis matches lang_tag sgn-CH-de ? NO
loc_range:sl_IT_NEDIS canonically matches lang_tag sgn_CH_DE ? NO
--------------
loc_range:sl_IT_Nedis matches lang_tag art-lojban ? NO
loc_range:sl_IT_NEDIS canonically matches lang_tag jbo ? NO
--------------
loc_range:sl_IT_Nedis matches lang_tag i-lux ? NO
loc_range:sl_IT_NEDIS canonically matches lang_tag lb ? NO
--------------
loc_range:sl_IT_Nedis matches lang_tag art-lojban ? NO
loc_range:sl_IT_NEDIS canonically matches lang_tag jbo ? NO
--------------
loc_range:sl_IT_Nedis matches lang_tag jbo ? NO
loc_range:sl_IT_NEDIS canonically matches lang_tag jbo ? NO
--------------
loc_range:sl_IT_Nedis matches lang_tag en_sl_IT ? NO
loc_range:sl_IT_NEDIS canonically matches lang_tag en_SL_IT ? NO
--------------
loc_range:jbo matches lang_tag de-DEVA ? NO
loc_range:jbo canonically matches lang_tag de_Deva ? NO
--------------
loc_range:jbo matches lang_tag de-DE-1996 ? NO
loc_range:jbo canonically matches lang_tag de_DE_1996 ? NO
--------------
loc_range:jbo matches lang_tag de-DE ? NO
loc_range:jbo canonically matches lang_tag de_DE ? NO
--------------
loc_range:jbo matches lang_tag zh_Hans ? NO
loc_range:jbo canonically matches lang_tag zh_Hans ? NO
--------------
loc_range:jbo matches lang_tag de-CH-1996 ? NO
loc_range:jbo canonically matches lang_tag de_CH_1996 ? NO
--------------
loc_range:jbo matches lang_tag sl_IT ? NO
loc_range:jbo canonically matches lang_tag sl_IT ? NO
--------------
loc_range:jbo matches lang_tag sl_IT_nedis-a-kirti-x-xyz ? NO
loc_range:jbo canonically matches lang_tag sl_IT_NEDIS_A_KIRTI_X_XYZ ? NO
--------------
loc_range:jbo matches lang_tag sl_IT_rozaj ? NO
loc_range:jbo canonically matches lang_tag sl_IT_ROZAJ ? NO
--------------
loc_range:jbo matches lang_tag sl_IT_NEDIS_ROJAZ_1901 ? NO
loc_range:jbo canonically matches lang_tag sl_IT_NEDIS_ROJAZ_1901 ? NO
--------------
loc_range:jbo matches lang_tag i-enochian ? NO
loc_range:jbo canonically matches lang_tag @x=i-enochian ? NO
--------------
loc_range:jbo matches lang_tag sgn-CH-de ? NO
loc_range:jbo canonically matches lang_tag sgn_CH_DE ? NO
--------------
loc_range:jbo matches lang_tag art-lojban ? NO
loc_range:jbo canonically matches lang_tag jbo ? YES
--------------
loc_range:jbo matches lang_tag i-lux ? NO
loc_range:jbo canonically matches lang_tag lb ? NO
--------------
loc_range:jbo matches lang_tag art-lojban ? NO
loc_range:jbo canonically matches lang_tag jbo ? YES
--------------
loc_range:jbo matches lang_tag jbo ? YES
loc_range:jbo canonically matches lang_tag jbo ? YES
--------------
loc_range:jbo matches lang_tag en_sl_IT ? NO
loc_range:jbo canonically matches lang_tag en_SL_IT ? NO
--------------
loc_range:art-lojban matches lang_tag de-DEVA ? NO
loc_range:jbo canonically matches lang_tag de_Deva ? NO
--------------
loc_range:art-lojban matches lang_tag de-DE-1996 ? NO
loc_range:jbo canonically matches lang_tag de_DE_1996 ? NO
--------------
loc_range:art-lojban matches lang_tag de-DE ? NO
loc_range:jbo canonically matches lang_tag de_DE ? NO
--------------
loc_range:art-lojban matches lang_tag zh_Hans ? NO
loc_range:jbo canonically matches lang_tag zh_Hans ? NO
--------------
loc_range:art-lojban matches lang_tag de-CH-1996 ? NO
loc_range:jbo canonically matches lang_tag de_CH_1996 ? NO
--------------
loc_range:art-lojban matches lang_tag sl_IT ? NO
loc_range:jbo canonically matches lang_tag sl_IT ? NO
--------------
loc_range:art-lojban matches lang_tag sl_IT_nedis-a-kirti-x-xyz ? NO
loc_range:jbo canonically matches lang_tag sl_IT_NEDIS_A_KIRTI_X_XYZ ? NO
--------------
loc_range:art-lojban matches lang_tag sl_IT_rozaj ? NO
loc_range:jbo canonically matches lang_tag sl_IT_ROZAJ ? NO
--------------
loc_range:art-lojban matches lang_tag sl_IT_NEDIS_ROJAZ_1901 ? NO
loc_range:jbo canonically matches lang_tag sl_IT_NEDIS_ROJAZ_1901 ? NO
--------------
loc_range:art-lojban matches lang_tag i-enochian ? NO
loc_range:jbo canonically matches lang_tag @x=i-enochian ? NO
--------------
loc_range:art-lojban matches lang_tag sgn-CH-de ? NO
loc_range:jbo canonically matches lang_tag sgn_CH_DE ? NO
--------------
loc_range:art-lojban matches lang_tag art-lojban ? YES
loc_range:jbo canonically matches lang_tag jbo ? YES
--------------
loc_range:art-lojban matches lang_tag i-lux ? NO
loc_range:jbo canonically matches lang_tag lb ? NO
--------------
loc_range:art-lojban matches lang_tag art-lojban ? YES
loc_range:jbo canonically matches lang_tag jbo ? YES
--------------
loc_range:art-lojban matches lang_tag jbo ? NO
loc_range:jbo canonically matches lang_tag jbo ? YES
--------------
loc_range:art-lojban matches lang_tag en_sl_IT ? NO
loc_range:jbo canonically matches lang_tag en_SL_IT ? NO
--------------
loc_range:sl_IT matches lang_tag de-DEVA ? NO
loc_range:sl_IT canonically matches lang_tag de_Deva ? NO
--------------
loc_range:sl_IT matches lang_tag de-DE-1996 ? NO
loc_range:sl_IT canonically matches lang_tag de_DE_1996 ? NO
--------------
loc_range:sl_IT matches lang_tag de-DE ? NO
loc_range:sl_IT canonically matches lang_tag de_DE ? NO
--------------
loc_range:sl_IT matches lang_tag zh_Hans ? NO
loc_range:sl_IT canonically matches lang_tag zh_Hans ? NO
--------------
loc_range:sl_IT matches lang_tag de-CH-1996 ? NO
loc_range:sl_IT canonically matches lang_tag de_CH_1996 ? NO
--------------
loc_range:sl_IT matches lang_tag sl_IT ? YES
loc_range:sl_IT canonically matches lang_tag sl_IT ? YES
--------------
loc_range:sl_IT matches lang_tag sl_IT_nedis-a-kirti-x-xyz ? YES
loc_range:sl_IT canonically matches lang_tag sl_IT_NEDIS_A_KIRTI_X_XYZ ? YES
--------------
loc_range:sl_IT matches lang_tag sl_IT_rozaj ? YES
loc_range:sl_IT canonically matches lang_tag sl_IT_ROZAJ ? YES
--------------
loc_range:sl_IT matches lang_tag sl_IT_NEDIS_ROJAZ_1901 ? YES
loc_range:sl_IT canonically matches lang_tag sl_IT_NEDIS_ROJAZ_1901 ? YES
--------------
loc_range:sl_IT matches lang_tag i-enochian ? NO
loc_range:sl_IT canonically matches lang_tag @x=i-enochian ? NO
--------------
loc_range:sl_IT matches lang_tag sgn-CH-de ? NO
loc_range:sl_IT canonically matches lang_tag sgn_CH_DE ? NO
--------------
loc_range:sl_IT matches lang_tag art-lojban ? NO
loc_range:sl_IT canonically matches lang_tag jbo ? NO
--------------
loc_range:sl_IT matches lang_tag i-lux ? NO
loc_range:sl_IT canonically matches lang_tag lb ? NO
--------------
loc_range:sl_IT matches lang_tag art-lojban ? NO
loc_range:sl_IT canonically matches lang_tag jbo ? NO
--------------
loc_range:sl_IT matches lang_tag jbo ? NO
loc_range:sl_IT canonically matches lang_tag jbo ? NO
--------------
loc_range:sl_IT matches lang_tag en_sl_IT ? NO
loc_range:sl_IT canonically matches lang_tag en_SL_IT ? NO
