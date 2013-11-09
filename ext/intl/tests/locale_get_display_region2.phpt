--TEST--
locale_get_display_region() icu >= 4.8 && icu < 51.2
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
<?php if(version_compare(INTL_ICU_VERSION, '4.8') < 0) print 'skip'; ?>
<?php if (version_compare(INTL_ICU_VERSION, '51.2') >=  0) die('skip for ICU < 51.2'); ?>
--FILE--
<?php

/*
 * Try getting the display_region for different locales
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
            $scr = ut_loc_get_display_region( $locale ,$disp_locale );
            $res_str .= "disp_locale=$disp_locale :  display_region=$scr";
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
disp_locale=en :  display_region=Ukraine
disp_locale=fr :  display_region=Ukraine
disp_locale=de :  display_region=Ukraine
-----------------
locale='root'
disp_locale=en :  display_region=
disp_locale=fr :  display_region=
disp_locale=de :  display_region=
-----------------
locale='uk@currency=EURO'
disp_locale=en :  display_region=
disp_locale=fr :  display_region=
disp_locale=de :  display_region=
-----------------
locale='Hindi'
disp_locale=en :  display_region=
disp_locale=fr :  display_region=
disp_locale=de :  display_region=
-----------------
locale='de'
disp_locale=en :  display_region=
disp_locale=fr :  display_region=
disp_locale=de :  display_region=
-----------------
locale='fr'
disp_locale=en :  display_region=
disp_locale=fr :  display_region=
disp_locale=de :  display_region=
-----------------
locale='ja'
disp_locale=en :  display_region=
disp_locale=fr :  display_region=
disp_locale=de :  display_region=
-----------------
locale='i-enochian'
disp_locale=en :  display_region=
disp_locale=fr :  display_region=
disp_locale=de :  display_region=
-----------------
locale='zh-Hant'
disp_locale=en :  display_region=
disp_locale=fr :  display_region=
disp_locale=de :  display_region=
-----------------
locale='zh-Hans'
disp_locale=en :  display_region=
disp_locale=fr :  display_region=
disp_locale=de :  display_region=
-----------------
locale='sr-Cyrl'
disp_locale=en :  display_region=
disp_locale=fr :  display_region=
disp_locale=de :  display_region=
-----------------
locale='sr-Latn'
disp_locale=en :  display_region=
disp_locale=fr :  display_region=
disp_locale=de :  display_region=
-----------------
locale='zh-Hans-CN'
disp_locale=en :  display_region=China
disp_locale=fr :  display_region=Chine
disp_locale=de :  display_region=China
-----------------
locale='sr-Latn-CS'
disp_locale=en :  display_region=Serbia [Aa]nd Montenegro
disp_locale=fr :  display_region=Serbie-et-Monténégro
disp_locale=de :  display_region=Serbien und Montenegro
-----------------
locale='sl-rozaj'
disp_locale=en :  display_region=(ROZAJ)?
disp_locale=fr :  display_region=(ROZAJ)?
disp_locale=de :  display_region=(ROZAJ)?
-----------------
locale='sl-nedis'
disp_locale=en :  display_region=(NEDIS)?
disp_locale=fr :  display_region=(NEDIS)?
disp_locale=de :  display_region=(NEDIS)?
-----------------
locale='de-CH-1901'
disp_locale=en :  display_region=Switzerland
disp_locale=fr :  display_region=Suisse
disp_locale=de :  display_region=Schweiz
-----------------
locale='sl-IT-nedis'
disp_locale=en :  display_region=Italy
disp_locale=fr :  display_region=Italie
disp_locale=de :  display_region=Italien
-----------------
locale='sl-Latn-IT-nedis'
disp_locale=en :  display_region=Italy
disp_locale=fr :  display_region=Italie
disp_locale=de :  display_region=Italien
-----------------
locale='de-DE'
disp_locale=en :  display_region=Germany
disp_locale=fr :  display_region=Allemagne
disp_locale=de :  display_region=Deutschland
-----------------
locale='en-US'
disp_locale=en :  display_region=United States
disp_locale=fr :  display_region=États-Unis
disp_locale=de :  display_region=Vereinigte Staaten
-----------------
locale='es-419'
disp_locale=en :  display_region=Latin America
disp_locale=fr :  display_region=Amérique latine
disp_locale=de :  display_region=Lateinamerika
-----------------
locale='de-CH-x-phonebk'
disp_locale=en :  display_region=Switzerland
disp_locale=fr :  display_region=Suisse
disp_locale=de :  display_region=Schweiz
-----------------
locale='az-Arab-x-AZE-derbend'
disp_locale=en :  display_region=X?
disp_locale=fr :  display_region=X?
disp_locale=de :  display_region=X?
-----------------
locale='zh-min'
disp_locale=en :  display_region=
disp_locale=fr :  display_region=
disp_locale=de :  display_region=
-----------------
locale='zh-min-nan-Hant-CN'
disp_locale=en :  display_region=MIN
disp_locale=fr :  display_region=MIN
disp_locale=de :  display_region=MIN
-----------------
locale='x-whatever'
disp_locale=en :  display_region=
disp_locale=fr :  display_region=
disp_locale=de :  display_region=
-----------------
locale='qaa-Qaaa-QM-x-southern'
disp_locale=en :  display_region=QM
disp_locale=fr :  display_region=QM
disp_locale=de :  display_region=QM
-----------------
locale='sr-Latn-QM'
disp_locale=en :  display_region=QM
disp_locale=fr :  display_region=QM
disp_locale=de :  display_region=QM
-----------------
locale='sr-Qaaa-CS'
disp_locale=en :  display_region=Serbia [Aa]nd Montenegro
disp_locale=fr :  display_region=Serbie-et-Monténégro
disp_locale=de :  display_region=Serbien und Montenegro
-----------------
locale='en-US-u-islamCal'
disp_locale=en :  display_region=United States
disp_locale=fr :  display_region=États-Unis
disp_locale=de :  display_region=Vereinigte Staaten
-----------------
locale='zh-CN-a-myExt-x-private'
disp_locale=en :  display_region=China
disp_locale=fr :  display_region=Chine
disp_locale=de :  display_region=China
-----------------
locale='en-a-myExt-b-another'
disp_locale=en :  display_region=A?
disp_locale=fr :  display_region=A?
disp_locale=de :  display_region=A?
-----------------
locale='de-419-DE'
disp_locale=en :  display_region=Latin America
disp_locale=fr :  display_region=Amérique latine
disp_locale=de :  display_region=Lateinamerika
-----------------
locale='a-DE'
disp_locale=en :  display_region=Germany
disp_locale=fr :  display_region=Allemagne
disp_locale=de :  display_region=Deutschland
-----------------
locale='ar-a-aaa-b-bbb-a-ccc'
disp_locale=en :  display_region=A?
disp_locale=fr :  display_region=A?
disp_locale=de :  display_region=A?
-----------------
