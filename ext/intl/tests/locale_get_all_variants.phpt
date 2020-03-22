--TEST--
locale_get_all_variants.phpt()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

/*
 * Try parsing different Locales
 * with Procedural and Object methods.
 */

function ut_main()
{
    $locales  = array(
        'sl_IT_nedis_KIRTI',
        'sl_IT_nedis-a-kirti-x-xyz',
        'sl_IT_rozaj',
        'sl_IT_NEDIS_ROJAZ_1901',
        'i-enochian',
        'zh-hakka',
        'zh-wuu',
        'i-tay',
        'sgn-BE-nl',
        'sgn-CH-de',
        'sl_IT_rozaj@currency=EUR'
    );
    $res_str = '';
    foreach($locales as $locale){
        $variants_arr = ut_loc_locale_get_all_variants( $locale);
        $res_str .= "$locale : variants ";
        if( $variants_arr){
            foreach($variants_arr as $variant){
                $res_str .= "'$variant',";
            }
        }else{
            $res_str .= "--none";
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
sl_IT_nedis_KIRTI : variants 'NEDIS','KIRTI',
sl_IT_nedis-a-kirti-x-xyz : variants 'NEDIS',
sl_IT_rozaj : variants 'ROZAJ',
sl_IT_NEDIS_ROJAZ_1901 : variants 'NEDIS','ROJAZ','1901',
i-enochian : variants --none
zh-hakka : variants --none
zh-wuu : variants --none
i-tay : variants --none
sgn-BE-nl : variants --none
sgn-CH-de : variants --none
sl_IT_rozaj@currency=EUR : variants 'ROZAJ',
