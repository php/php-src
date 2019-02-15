--TEST--
locale_get_script()
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
        $scr = ut_loc_get_script( $locale);
        $res_str .= "$locale:  script='$scr'";
        $res_str .= "\n";
    }

    return $res_str;

}

include_once( 'ut_common.inc' );
ut_run();

?>
--EXPECT--
uk-ua_CALIFORNIA@currency=;currency=GRN:  script=''
root:  script=''
uk@currency=EURO:  script=''
Hindi:  script=''
de:  script=''
fr:  script=''
ja:  script=''
i-enochian:  script=''
zh-Hant:  script='Hant'
zh-Hans:  script='Hans'
sr-Cyrl:  script='Cyrl'
sr-Latn:  script='Latn'
zh-Hans-CN:  script='Hans'
sr-Latn-CS:  script='Latn'
sl-rozaj:  script=''
sl-nedis:  script=''
de-CH-1901:  script=''
sl-IT-nedis:  script=''
sl-Latn-IT-nedis:  script='Latn'
de-DE:  script=''
en-US:  script=''
es-419:  script=''
de-CH-x-phonebk:  script=''
az-Arab-x-AZE-derbend:  script='Arab'
zh-min:  script=''
zh-min-nan-Hant-CN:  script=''
x-whatever:  script=''
qaa-Qaaa-QM-x-southern:  script='Qaaa'
sr-Latn-QM:  script='Latn'
sr-Qaaa-CS:  script='Qaaa'
en-US-u-islamCal:  script=''
zh-CN-a-myExt-x-private:  script=''
en-a-myExt-b-another:  script=''
de-419-DE:  script=''
a-DE:  script=''
ar-a-aaa-b-bbb-a-ccc:  script=''
