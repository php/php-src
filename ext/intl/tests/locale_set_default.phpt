--TEST--
locale_set_default($locale)
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

/*
 * Try setting the default Locale with different locales
 * with Procedural and Object methods.
 */

function ut_main()
{
    $res_str = '';

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
//Tags that use extensions (examples ONLY: extensions MUST be defined
//   by revision or update to this document or by RFC):
        'en-US-u-islamCal',
        'zh-CN-a-myExt-x-private',
        'en-a-myExt-b-another',
//Some Invalid Tags:
        'de-419-DE',
        'a-DE',
        'ar-a-aaa-b-bbb-a-ccc'
    );

/*
    $locales = array(
        'es'
    );
*/
    $res_str = '';

    foreach( $locales as $locale )
    {
        $isSuccessful = ut_loc_set_default( $locale);
    if ($isSuccessful ){
        $lang = ut_loc_get_default( );
        $res_str .= "$locale: set locale '$lang'";
    }
    else{
        $res_str .= "$locale: Error in set locale";
    }
        $res_str .= "\n";
    }

    return $res_str;

}

include_once( 'ut_common.inc' );
ut_run();

?>
--EXPECT--
uk-ua_CALIFORNIA@currency=;currency=GRN: set locale 'uk-ua_CALIFORNIA@currency=;currency=GRN'
root: set locale 'root'
uk@currency=EURO: set locale 'uk@currency=EURO'
Hindi: set locale 'Hindi'
de: set locale 'de'
fr: set locale 'fr'
ja: set locale 'ja'
i-enochian: set locale 'i-enochian'
zh-Hant: set locale 'zh-Hant'
zh-Hans: set locale 'zh-Hans'
sr-Cyrl: set locale 'sr-Cyrl'
sr-Latn: set locale 'sr-Latn'
zh-Hans-CN: set locale 'zh-Hans-CN'
sr-Latn-CS: set locale 'sr-Latn-CS'
sl-rozaj: set locale 'sl-rozaj'
sl-nedis: set locale 'sl-nedis'
de-CH-1901: set locale 'de-CH-1901'
sl-IT-nedis: set locale 'sl-IT-nedis'
sl-Latn-IT-nedis: set locale 'sl-Latn-IT-nedis'
de-DE: set locale 'de-DE'
en-US: set locale 'en-US'
es-419: set locale 'es-419'
de-CH-x-phonebk: set locale 'de-CH-x-phonebk'
az-Arab-x-AZE-derbend: set locale 'az-Arab-x-AZE-derbend'
zh-min: set locale 'zh-min'
zh-min-nan-Hant-CN: set locale 'zh-min-nan-Hant-CN'
x-whatever: set locale 'x-whatever'
qaa-Qaaa-QM-x-southern: set locale 'qaa-Qaaa-QM-x-southern'
sr-Latn-QM: set locale 'sr-Latn-QM'
sr-Qaaa-CS: set locale 'sr-Qaaa-CS'
en-US-u-islamCal: set locale 'en-US-u-islamCal'
zh-CN-a-myExt-x-private: set locale 'zh-CN-a-myExt-x-private'
en-a-myExt-b-another: set locale 'en-a-myExt-b-another'
de-419-DE: set locale 'de-419-DE'
a-DE: set locale 'a-DE'
ar-a-aaa-b-bbb-a-ccc: set locale 'ar-a-aaa-b-bbb-a-ccc'
