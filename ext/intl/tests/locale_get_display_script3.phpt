--TEST--
locale_get_display_script()  icu >= 49
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
<?php if(version_compare(INTL_ICU_VERSION, '49') < 0 || version_compare(INTL_ICU_VERSION, '52.1') >= 0) print 'skip'; ?>
--FILE--
<?php

/*
 * Try getting the display_script for different locales
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
            $scr = ut_loc_get_display_script( $locale ,$disp_locale );
            $res_str .= "disp_locale=$disp_locale :  display_script=$scr";
            $res_str .= "\n";
        }
        $res_str .= "-----------------\n";
    }

    return $res_str;

}

include_once( 'ut_common.inc' );
ut_run();

?>
--EXPECT--
locale='uk-ua_CALIFORNIA@currency=;currency=GRN'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
locale='root'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
locale='uk@currency=EURO'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
locale='Hindi'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
locale='de'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
locale='fr'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
locale='ja'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
locale='i-enochian'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
locale='zh-Hant'
disp_locale=en :  display_script=Traditional Han
disp_locale=fr :  display_script=chinois traditionnel
disp_locale=de :  display_script=Traditionelles Chinesisch
-----------------
locale='zh-Hans'
disp_locale=en :  display_script=Simplified Han
disp_locale=fr :  display_script=chinois simplifié
disp_locale=de :  display_script=Vereinfachtes Chinesisch
-----------------
locale='sr-Cyrl'
disp_locale=en :  display_script=Cyrillic
disp_locale=fr :  display_script=cyrillique
disp_locale=de :  display_script=Kyrillisch
-----------------
locale='sr-Latn'
disp_locale=en :  display_script=Latin
disp_locale=fr :  display_script=latin
disp_locale=de :  display_script=Lateinisch
-----------------
locale='zh-Hans-CN'
disp_locale=en :  display_script=Simplified Han
disp_locale=fr :  display_script=chinois simplifié
disp_locale=de :  display_script=Vereinfachtes Chinesisch
-----------------
locale='sr-Latn-CS'
disp_locale=en :  display_script=Latin
disp_locale=fr :  display_script=latin
disp_locale=de :  display_script=Lateinisch
-----------------
locale='sl-rozaj'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
locale='sl-nedis'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
locale='de-CH-1901'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
locale='sl-IT-nedis'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
locale='sl-Latn-IT-nedis'
disp_locale=en :  display_script=Latin
disp_locale=fr :  display_script=latin
disp_locale=de :  display_script=Lateinisch
-----------------
locale='de-DE'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
locale='en-US'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
locale='es-419'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
locale='de-CH-x-phonebk'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
locale='az-Arab-x-AZE-derbend'
disp_locale=en :  display_script=Arabic
disp_locale=fr :  display_script=arabe
disp_locale=de :  display_script=Arabisch
-----------------
locale='zh-min'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
locale='zh-min-nan-Hant-CN'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
locale='x-whatever'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
locale='qaa-Qaaa-QM-x-southern'
disp_locale=en :  display_script=Qaaa
disp_locale=fr :  display_script=Qaaa
disp_locale=de :  display_script=Qaaa
-----------------
locale='sr-Latn-QM'
disp_locale=en :  display_script=Latin
disp_locale=fr :  display_script=latin
disp_locale=de :  display_script=Lateinisch
-----------------
locale='sr-Qaaa-CS'
disp_locale=en :  display_script=Qaaa
disp_locale=fr :  display_script=Qaaa
disp_locale=de :  display_script=Qaaa
-----------------
locale='en-US-u-islamCal'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
locale='zh-CN-a-myExt-x-private'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
locale='en-a-myExt-b-another'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
locale='de-419-DE'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
locale='a-DE'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
locale='ar-a-aaa-b-bbb-a-ccc'
disp_locale=en :  display_script=
disp_locale=fr :  display_script=
disp_locale=de :  display_script=
-----------------
