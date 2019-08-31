--TEST--
locale_get_keywords() icu >= 4.8
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip intl extension not loaded'; ?>
--FILE--
<?php

/*
 * Try getting the keywords for different locales
 * with Procedural and Object methods.
 */

function ut_main()
{
    $res_str = '';

    $locales = array(
		"de_DE@currency=EUR;collation=PHONEBOOK",
        'uk-ua_CALIFORNIA@currency=GRN'
	);

    $locales = array(
	'de_DE@currency=EUR;collation=PHONEBOOK',
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
        $keywords_arr = ut_loc_get_keywords( $locale);
        $res_str .= "$locale: ";
		if( $keywords_arr){
			foreach( $keywords_arr as $key => $value){
        			$res_str .= "Key is $key and Value is $value \n";
			}
		}
		else{
			$res_str .= "No keywords found.";
		}
        $res_str .= "\n";
    }

    $res_str .= "\n";
    return $res_str;

}

include_once( 'ut_common.inc' );
ut_run();

?>
--EXPECT--
de_DE@currency=EUR;collation=PHONEBOOK: Key is collation and Value is PHONEBOOK 
Key is currency and Value is EUR 

root: No keywords found.
uk@currency=EURO: Key is currency and Value is EURO 

Hindi: No keywords found.
de: No keywords found.
fr: No keywords found.
ja: No keywords found.
i-enochian: Key is x and Value is i-enochian 

zh-Hant: No keywords found.
zh-Hans: No keywords found.
sr-Cyrl: No keywords found.
sr-Latn: No keywords found.
zh-Hans-CN: No keywords found.
sr-Latn-CS: No keywords found.
sl-rozaj: No keywords found.
sl-nedis: No keywords found.
de-CH-1901: No keywords found.
sl-IT-nedis: No keywords found.
sl-Latn-IT-nedis: No keywords found.
de-DE: No keywords found.
en-US: No keywords found.
es-419: No keywords found.
de-CH-x-phonebk: Key is x and Value is phonebk 

az-Arab-x-AZE-derbend: Key is x and Value is aze-derbend 

zh-min: No keywords found.
zh-min-nan-Hant-CN: No keywords found.
x-whatever: Key is x and Value is whatever 

qaa-Qaaa-QM-x-southern: Key is x and Value is southern 

sr-Latn-QM: No keywords found.
sr-Qaaa-CS: No keywords found.
en-US-u-islamCal: Key is attribute and Value is islamcal 

zh-CN-a-myExt-x-private: Key is a and Value is myext 
Key is x and Value is private 

en-a-myExt-b-another: Key is a and Value is myext 
Key is b and Value is another 

de-419-DE: No keywords found.
a-DE: No keywords found.
ar-a-aaa-b-bbb-a-ccc: Key is a and Value is aaa 
Key is b and Value is bbb
