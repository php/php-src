--TEST--
locale_get_language()
--EXTENSIONS--
intl
--FILE--
<?php

/*
 * Try getting the language for different locales
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
        $lang = ut_loc_get_language( $locale);
        $res_str .= "$locale:  language='$lang'";
        $res_str .= "\n";

        // Also test the alias Locale::getPrimaryLanguage().
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
uk-ua_CALIFORNIA@currency=;currency=GRN:  language='uk'
uk-ua_CALIFORNIA@currency=;currency=GRN:  primary_language='uk'
root:  language='%S'
root:  primary_language='%S'
uk@currency=EURO:  language='uk'
uk@currency=EURO:  primary_language='uk'
Hindi:  language='hindi'
Hindi:  primary_language='hindi'
de:  language='de'
de:  primary_language='de'
fr:  language='fr'
fr:  primary_language='fr'
ja:  language='ja'
ja:  primary_language='ja'
i-enochian:  language='i-enochian'
i-enochian:  primary_language='i-enochian'
zh-Hant:  language='zh'
zh-Hant:  primary_language='zh'
zh-Hans:  language='zh'
zh-Hans:  primary_language='zh'
sr-Cyrl:  language='sr'
sr-Cyrl:  primary_language='sr'
sr-Latn:  language='sr'
sr-Latn:  primary_language='sr'
zh-Hans-CN:  language='zh'
zh-Hans-CN:  primary_language='zh'
sr-Latn-CS:  language='sr'
sr-Latn-CS:  primary_language='sr'
sl-rozaj:  language='sl'
sl-rozaj:  primary_language='sl'
sl-nedis:  language='sl'
sl-nedis:  primary_language='sl'
de-CH-1901:  language='de'
de-CH-1901:  primary_language='de'
sl-IT-nedis:  language='sl'
sl-IT-nedis:  primary_language='sl'
sl-Latn-IT-nedis:  language='sl'
sl-Latn-IT-nedis:  primary_language='sl'
de-DE:  language='de'
de-DE:  primary_language='de'
en-US:  language='en'
en-US:  primary_language='en'
es-419:  language='es'
es-419:  primary_language='es'
de-CH-x-phonebk:  language='de'
de-CH-x-phonebk:  primary_language='de'
az-Arab-x-AZE-derbend:  language='az'
az-Arab-x-AZE-derbend:  primary_language='az'
zh-min:  language='zh-min'
zh-min:  primary_language='zh-min'
zh-min-nan-Hant-CN:  language='zh'
zh-min-nan-Hant-CN:  primary_language='zh'
qaa-Qaaa-QM-x-southern:  language='qaa'
qaa-Qaaa-QM-x-southern:  primary_language='qaa'
sr-Latn-QM:  language='sr'
sr-Latn-QM:  primary_language='sr'
sr-Qaaa-CS:  language='sr'
sr-Qaaa-CS:  primary_language='sr'
en-US-u-islamCal:  language='en'
en-US-u-islamCal:  primary_language='en'
zh-CN-a-myExt-x-private:  language='zh'
zh-CN-a-myExt-x-private:  primary_language='zh'
en-a-myExt-b-another:  language='en'
en-a-myExt-b-another:  primary_language='en'
de-419-DE:  language='de'
de-419-DE:  primary_language='de'
a-DE:  language='a'
a-DE:  primary_language='a'
ar-a-aaa-b-bbb-a-ccc:  language='ar'
ar-a-aaa-b-bbb-a-ccc:  primary_language='ar'
