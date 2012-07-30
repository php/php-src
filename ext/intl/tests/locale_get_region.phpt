--TEST--
locale_get_region()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

/*
 * Try getting the region for different locales
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
        $scr = ut_loc_get_region( $locale);
        $res_str .= "$locale:  region='$scr'";
        $res_str .= "\n";
    }

    return $res_str;

}

include_once( 'ut_common.inc' );
ut_run();

?>
--EXPECTF--
uk-ua_CALIFORNIA@currency=;currency=GRN:  region='UA'
root:  region=''
uk@currency=EURO:  region=''
Hindi:  region=''
de:  region=''
fr:  region=''
ja:  region=''
i-enochian:  region=''
zh-Hant:  region=''
zh-Hans:  region=''
sr-Cyrl:  region=''
sr-Latn:  region=''
zh-Hans-CN:  region='CN'
sr-Latn-CS:  region='CS'
sl-rozaj:  region='%r(ROZAJ)?%r'
sl-nedis:  region='%r(NEDIS)?%r'
de-CH-1901:  region='CH'
sl-IT-nedis:  region='IT'
sl-Latn-IT-nedis:  region='IT'
de-DE:  region='DE'
en-US:  region='US'
es-419:  region='419'
de-CH-x-phonebk:  region='CH'
az-Arab-x-AZE-derbend:  region='%r(X)?%r'
zh-min:  region=''
zh-min-nan-Hant-CN:  region='MIN'
x-whatever:  region=''
qaa-Qaaa-QM-x-southern:  region='QM'
sr-Latn-QM:  region='QM'
sr-Qaaa-CS:  region='CS'
en-US-u-islamCal:  region='US'
zh-CN-a-myExt-x-private:  region='CN'
en-a-myExt-b-another:  region='%r(A)?%r'
de-419-DE:  region='419'
a-DE:  region='DE'
ar-a-aaa-b-bbb-a-ccc:  region='%r(A)?%r'
