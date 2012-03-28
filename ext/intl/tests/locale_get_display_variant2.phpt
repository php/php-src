--TEST--
locale_get_display_variant() icu >= 4.8
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
<?php if(version_compare(INTL_ICU_VERSION, '4.8') < 0) print 'skip'; ?>
--FILE--
<?php

/*
 * Try getting the display_variant for different locales
 * with Procedural and Object methods.
 */

function ut_main()
{
    $res_str = '';

    $disp_locales=array('en','fr','de');

    $locales = array(
        'uk-ua_CALIFORNIA@currency=;currency=GRN',
        'root',
        'uk@currency=EURO',
        'Hindi',
//Simple language subtag
        'de',
        'fr',
        'ja',
        'i-enochian', //(example of a grandfathered tag)
//Language subtag plus Script subtag:
        'zh-Hant',
        'zh-Hans',
        'sr-Cyrl',
        'sr-Latn',
//Language-Script-Region
        'zh-Hans-CN',
        'sr-Latn-CS',
//Language-Variant
        'sl-rozaj',
        'sl-nedis',
//Language-Region-Variant
        'de-CH-1901',
        'sl-IT-nedis',
//Language-Script-Region-Variant
        'sl-Latn-IT-nedis',
//Language-Region:
        'de-DE',
        'en-US',
        'es-419',
//Private use subtags:
        'de-CH-x-phonebk',
        'az-Arab-x-AZE-derbend',
//Extended language subtags
        'zh-min',
        'zh-min-nan-Hant-CN',
//Private use registry values
        'x-whatever',
        'qaa-Qaaa-QM-x-southern',
        'sr-Latn-QM',
        'sr-Qaaa-CS',
/*Tags that use extensions (examples ONLY: extensions MUST be defined
   by revision or update to this document or by RFC): */
        'en-US-u-islamCal',
        'zh-CN-a-myExt-x-private',
        'en-a-myExt-b-another',
//Some Invalid Tags:
        'de-419-DE',
        'a-DE',
        'ar-a-aaa-b-bbb-a-ccc'
    );


    $res_str = '';

    foreach( $locales as $locale )
    {
        $res_str .= "locale='$locale'\n";
        foreach( $disp_locales as $disp_locale )
        {
            $scr = ut_loc_get_display_variant( $locale ,$disp_locale );
            $res_str .= "disp_locale=$disp_locale :  display_variant=$scr";
            $res_str .= "\n";
        }
        $res_str .= "-----------------\n";
    }

    return $res_str;

}

include_once( 'ut_common.inc' );
ut_run();

?>
--EXPECTREGEX--
locale='uk-ua_CALIFORNIA@currency=;currency=GRN'
disp_locale=en :  display_variant=CALIFORNIA
disp_locale=fr :  display_variant=CALIFORNIA
disp_locale=de :  display_variant=CALIFORNIA
-----------------
locale='root'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='uk@currency=EURO'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='Hindi'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='de'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='fr'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='ja'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='i-enochian'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='zh-Hant'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='zh-Hans'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='sr-Cyrl'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='sr-Latn'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='zh-Hans-CN'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='sr-Latn-CS'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='sl-rozaj'
disp_locale=en :  display_variant=Resian
disp_locale=fr :  display_variant=dialecte de Resia
disp_locale=de :  display_variant=Resianisch
-----------------
locale='sl-nedis'
disp_locale=en :  display_variant=Natisone dialect
disp_locale=fr :  display_variant=dialecte de Natisone
disp_locale=de :  display_variant=Natisone-Dialekt
-----------------
locale='de-CH-1901'
disp_locale=en :  display_variant=Traditional German orthography
disp_locale=fr :  display_variant=orthographe allemande traditionnelle
disp_locale=de :  display_variant=(1901|[Aa]lte deutsche Rechtschreibung)
-----------------
locale='sl-IT-nedis'
disp_locale=en :  display_variant=Natisone dialect
disp_locale=fr :  display_variant=dialecte de Natisone
disp_locale=de :  display_variant=(NEDIS|Natisone-Dialekt)
-----------------
locale='sl-Latn-IT-nedis'
disp_locale=en :  display_variant=Natisone dialect
disp_locale=fr :  display_variant=dialecte de Natisone
disp_locale=de :  display_variant=(NEDIS|Natisone-Dialekt)
-----------------
locale='de-DE'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='en-US'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='es-419'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='de-CH-x-phonebk'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='az-Arab-x-AZE-derbend'
disp_locale=en :  display_variant=(AZE_DERBEND)?
disp_locale=fr :  display_variant=(AZE_DERBEND)?
disp_locale=de :  display_variant=(AZE_DERBEND)?
-----------------
locale='zh-min'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='zh-min-nan-Hant-CN'
disp_locale=en :  display_variant=NAN_HANT_CN
disp_locale=fr :  display_variant=NAN_HANT_CN
disp_locale=de :  display_variant=NAN_HANT_CN
-----------------
locale='x-whatever'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='qaa-Qaaa-QM-x-southern'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='sr-Latn-QM'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='sr-Qaaa-CS'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='en-US-u-islamCal'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='zh-CN-a-myExt-x-private'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='en-a-myExt-b-another'
disp_locale=en :  display_variant=(MYEXT_B_ANOTHER)?
disp_locale=fr :  display_variant=(MYEXT_B_ANOTHER)?
disp_locale=de :  display_variant=(MYEXT_B_ANOTHER)?
-----------------
locale='de-419-DE'
disp_locale=en :  display_variant=DE
disp_locale=fr :  display_variant=DE
disp_locale=de :  display_variant=DE
-----------------
locale='a-DE'
disp_locale=en :  display_variant=
disp_locale=fr :  display_variant=
disp_locale=de :  display_variant=
-----------------
locale='ar-a-aaa-b-bbb-a-ccc'
disp_locale=en :  display_variant=(AAA_B_BBB_A_CCC)?
disp_locale=fr :  display_variant=(AAA_B_BBB_A_CCC)?
disp_locale=de :  display_variant=(AAA_B_BBB_A_CCC)?
-----------------
