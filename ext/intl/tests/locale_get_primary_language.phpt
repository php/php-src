--TEST--
locale_get_primary_language()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

/*
 * Try getting the prmary language for different locales
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
        $lang = ut_loc_get_primary_language( $locale);
        $res_str .= "$locale:  primary_language='$lang'";
        $res_str .= "\n";
    }

    return $res_str;

}

include_once( 'ut_common.inc' );
ut_run();

?>
--EXPECTF--
uk-ua_CALIFORNIA@currency=;currency=GRN:  primary_language='uk'
root:  primary_language='%S'
uk@currency=EURO:  primary_language='uk'
Hindi:  primary_language='hindi'
de:  primary_language='de'
fr:  primary_language='fr'
ja:  primary_language='ja'
i-enochian:  primary_language='i-enochian'
zh-Hant:  primary_language='zh'
zh-Hans:  primary_language='zh'
sr-Cyrl:  primary_language='sr'
sr-Latn:  primary_language='sr'
zh-Hans-CN:  primary_language='zh'
sr-Latn-CS:  primary_language='sr'
sl-rozaj:  primary_language='sl'
sl-nedis:  primary_language='sl'
de-CH-1901:  primary_language='de'
sl-IT-nedis:  primary_language='sl'
sl-Latn-IT-nedis:  primary_language='sl'
de-DE:  primary_language='de'
en-US:  primary_language='en'
es-419:  primary_language='es'
de-CH-x-phonebk:  primary_language='de'
az-Arab-x-AZE-derbend:  primary_language='az'
zh-min:  primary_language='zh-min'
zh-min-nan-Hant-CN:  primary_language='zh'
qaa-Qaaa-QM-x-southern:  primary_language='qaa'
sr-Latn-QM:  primary_language='sr'
sr-Qaaa-CS:  primary_language='sr'
en-US-u-islamCal:  primary_language='en'
zh-CN-a-myExt-x-private:  primary_language='zh'
en-a-myExt-b-another:  primary_language='en'
de-419-DE:  primary_language='de'
a-DE:  primary_language='a'
ar-a-aaa-b-bbb-a-ccc:  primary_language='ar'
