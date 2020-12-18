--TEST--
locale_get_display_language() icu < 68.1
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
<?php if (version_compare(INTL_ICU_VERSION, '68.1') >=  0) die('skip for ICU < 68.1'); ?>
--FILE--
<?php

/*
 * Try getting the display_language for different locales
 * with Procedural and Object methods.
 */

function ut_main()
{
    $res_str='';

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
        'art-lojban', //(example of a grandfathered tag)
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
            $scr = ut_loc_get_display_language( $locale ,$disp_locale );
            $res_str .= "disp_locale=$disp_locale :  display_language=$scr";
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
disp_locale=en :  display_language=Ukrainian
disp_locale=fr :  display_language=ukrainien
disp_locale=de :  display_language=Ukrainisch
-----------------
locale='root'
disp_locale=en :  display_language=(Root|Unknown language)
disp_locale=fr :  display_language=(racine|langue indéterminée)
disp_locale=de :  display_language=([rR]oot|Unbekannte Sprache)
-----------------
locale='uk@currency=EURO'
disp_locale=en :  display_language=Ukrainian
disp_locale=fr :  display_language=ukrainien
disp_locale=de :  display_language=Ukrainisch
-----------------
locale='Hindi'
disp_locale=en :  display_language=hindi
disp_locale=fr :  display_language=hindi
disp_locale=de :  display_language=hindi
-----------------
locale='de'
disp_locale=en :  display_language=German
disp_locale=fr :  display_language=allemand
disp_locale=de :  display_language=Deutsch
-----------------
locale='fr'
disp_locale=en :  display_language=French
disp_locale=fr :  display_language=français
disp_locale=de :  display_language=Französisch
-----------------
locale='ja'
disp_locale=en :  display_language=Japanese
disp_locale=fr :  display_language=japonais
disp_locale=de :  display_language=Japanisch
-----------------
locale='i-enochian'
disp_locale=en :  display_language=i-enochian
disp_locale=fr :  display_language=i-enochian
disp_locale=de :  display_language=i-enochian
-----------------
locale='art-lojban'
disp_locale=en :  display_language=Lojban
disp_locale=fr :  display_language=lojban
disp_locale=de :  display_language=Lojban
-----------------
locale='zh-Hant'
disp_locale=en :  display_language=Chinese
disp_locale=fr :  display_language=chinois
disp_locale=de :  display_language=Chinesisch
-----------------
locale='zh-Hans'
disp_locale=en :  display_language=Chinese
disp_locale=fr :  display_language=chinois
disp_locale=de :  display_language=Chinesisch
-----------------
locale='sr-Cyrl'
disp_locale=en :  display_language=Serbian
disp_locale=fr :  display_language=serbe
disp_locale=de :  display_language=Serbisch
-----------------
locale='sr-Latn'
disp_locale=en :  display_language=Serbian
disp_locale=fr :  display_language=serbe
disp_locale=de :  display_language=Serbisch
-----------------
locale='zh-Hans-CN'
disp_locale=en :  display_language=Chinese
disp_locale=fr :  display_language=chinois
disp_locale=de :  display_language=Chinesisch
-----------------
locale='sr-Latn-CS'
disp_locale=en :  display_language=Serbian
disp_locale=fr :  display_language=serbe
disp_locale=de :  display_language=Serbisch
-----------------
locale='sl-rozaj'
disp_locale=en :  display_language=Slovenian
disp_locale=fr :  display_language=slovène
disp_locale=de :  display_language=Slowenisch
-----------------
locale='sl-nedis'
disp_locale=en :  display_language=Slovenian
disp_locale=fr :  display_language=slovène
disp_locale=de :  display_language=Slowenisch
-----------------
locale='de-CH-1901'
disp_locale=en :  display_language=German
disp_locale=fr :  display_language=allemand
disp_locale=de :  display_language=Deutsch
-----------------
locale='sl-IT-nedis'
disp_locale=en :  display_language=Slovenian
disp_locale=fr :  display_language=slovène
disp_locale=de :  display_language=Slowenisch
-----------------
locale='sl-Latn-IT-nedis'
disp_locale=en :  display_language=Slovenian
disp_locale=fr :  display_language=slovène
disp_locale=de :  display_language=Slowenisch
-----------------
locale='de-DE'
disp_locale=en :  display_language=German
disp_locale=fr :  display_language=allemand
disp_locale=de :  display_language=Deutsch
-----------------
locale='en-US'
disp_locale=en :  display_language=English
disp_locale=fr :  display_language=anglais
disp_locale=de :  display_language=Englisch
-----------------
locale='es-419'
disp_locale=en :  display_language=Spanish
disp_locale=fr :  display_language=espagnol
disp_locale=de :  display_language=Spanisch
-----------------
locale='de-CH-x-phonebk'
disp_locale=en :  display_language=German
disp_locale=fr :  display_language=allemand
disp_locale=de :  display_language=Deutsch
-----------------
locale='az-Arab-x-AZE-derbend'
disp_locale=en :  display_language=Azerbaijani
disp_locale=fr :  display_language=azéri
disp_locale=de :  display_language=Aserbaidschanisch
-----------------
locale='zh-min'
disp_locale=en :  display_language=Chinese
disp_locale=fr :  display_language=chinois
disp_locale=de :  display_language=Chinesisch
-----------------
locale='zh-min-nan-Hant-CN'
disp_locale=en :  display_language=Chinese
disp_locale=fr :  display_language=chinois
disp_locale=de :  display_language=Chinesisch
-----------------
locale='x-whatever'
disp_locale=en :  display_language=x-whatever
disp_locale=fr :  display_language=x-whatever
disp_locale=de :  display_language=x-whatever
-----------------
locale='qaa-Qaaa-QM-x-southern'
disp_locale=en :  display_language=qaa
disp_locale=fr :  display_language=qaa
disp_locale=de :  display_language=qaa
-----------------
locale='sr-Latn-QM'
disp_locale=en :  display_language=Serbian
disp_locale=fr :  display_language=serbe
disp_locale=de :  display_language=Serbisch
-----------------
locale='sr-Qaaa-CS'
disp_locale=en :  display_language=Serbian
disp_locale=fr :  display_language=serbe
disp_locale=de :  display_language=Serbisch
-----------------
locale='en-US-u-islamCal'
disp_locale=en :  display_language=English
disp_locale=fr :  display_language=anglais
disp_locale=de :  display_language=Englisch
-----------------
locale='zh-CN-a-myExt-x-private'
disp_locale=en :  display_language=Chinese
disp_locale=fr :  display_language=chinois
disp_locale=de :  display_language=Chinesisch
-----------------
locale='en-a-myExt-b-another'
disp_locale=en :  display_language=English
disp_locale=fr :  display_language=anglais
disp_locale=de :  display_language=Englisch
-----------------
locale='de-419-DE'
disp_locale=en :  display_language=German
disp_locale=fr :  display_language=allemand
disp_locale=de :  display_language=Deutsch
-----------------
locale='a-DE'
disp_locale=en :  display_language=a
disp_locale=fr :  display_language=a
disp_locale=de :  display_language=a
-----------------
locale='ar-a-aaa-b-bbb-a-ccc'
disp_locale=en :  display_language=Arabic
disp_locale=fr :  display_language=arabe
disp_locale=de :  display_language=Arabisch
-----------------
