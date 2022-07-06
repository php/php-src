--TEST--
locale_compose_locale()
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
    $loc_parts_arr1 = array(
        Locale::LANG_TAG =>'sl' ,
        Locale::SCRIPT_TAG =>'Latn' ,
        Locale::REGION_TAG =>'IT'
    );
    $loc_parts_arr2 = array(
        Locale::LANG_TAG =>'de' ,
        Locale::REGION_TAG =>'DE'
    );
    $loc_parts_arr3 = array(
        Locale::LANG_TAG =>'hi'
    );
    $loc_parts_arr4 = array(
        Locale::LANG_TAG =>'zh' ,
        Locale::SCRIPT_TAG =>'Hans' ,
        Locale::REGION_TAG =>'CN'
    );
    $loc_parts_arr5 = array(
        Locale::LANG_TAG =>'es' ,
        Locale::SCRIPT_TAG =>'Hans' ,
        Locale::REGION_TAG =>'CN'
    );
    $loc_parts_arr6 = array(
        Locale::LANG_TAG =>'en' ,
        Locale::SCRIPT_TAG =>'Hans' ,
        Locale::REGION_TAG =>'CN',
        Locale::VARIANT_TAG.'14' =>'rozaj' ,
        'variant1'=>'nedis'
    );
    $loc_parts_arr7 = array(
        Locale::LANG_TAG =>'en' ,
        Locale::SCRIPT_TAG =>'Hans' ,
        Locale::REGION_TAG =>'CN',
        'variant14'=>'rozaj' ,
        'variant1'=>'nedis' ,
        'extlang0'=>'lng' ,
        'extlang1'=>'ing'
    );
    $loc_parts_arr8 = array(
        Locale::LANG_TAG =>'en' ,
        Locale::SCRIPT_TAG =>'Hans' ,
        Locale::REGION_TAG =>'CN',
        'variant14'=>'rozaj' ,
        'variant1'=>'nedis' ,
        'extlang0'=>'lng' ,
        'extlang1'=>'ing',
        'private7'=>'prv1' ,
        'private9'=>'prv2'
    );
    $loc_parts_arr9 = array(
        Locale::REGION_TAG =>'DE'
    );
    $loc_parts_arr10 = array(
        Locale::LANG_TAG => 45,
        Locale::REGION_TAG => false,
        Locale::SCRIPT_TAG => 15
    );
    $loc_parts_arr11 = array(
        Locale::LANG_TAG =>'de'  ,
        Locale::REGION_TAG =>'DE',
        'private0' => 13,
        'variant1' => array(),
        'extlang2' => false
    );
        $loc_parts_arr12 = array(
                Locale::LANG_TAG =>'en' ,
                Locale::SCRIPT_TAG =>'Hans' ,
                Locale::REGION_TAG =>'CN',
                Locale::VARIANT_TAG => array('nedis', 'rozaj'),
        Locale::PRIVATE_TAG => array('prv1', 'prv2'),
        Locale::EXTLANG_TAG => array('lng', 'ing')
        );


    $loc_parts_arr = array(
        'loc1' => $loc_parts_arr1	,
        'loc2' => $loc_parts_arr2	,
        'loc3' => $loc_parts_arr3	,
        'loc4' => $loc_parts_arr4	,
        'loc5' => $loc_parts_arr5	,
        'loc6' => $loc_parts_arr6	,
        'loc7' => $loc_parts_arr7	,
        'loc8' => $loc_parts_arr8	,
        'loc9' => $loc_parts_arr9	,
        'loc10' => $loc_parts_arr10	,
        'loc11' => $loc_parts_arr11	,
        'loc12' => $loc_parts_arr12
    );

    error_reporting( E_ERROR );

    $cnt  = 0;
    $res_str = '';
    foreach($loc_parts_arr as $key => $value ){
        $res_str .= "\n------------";
        $res_str .= "\nInput Array name is : loc".(++$cnt) ;
/*
        foreach($value as $valKey => $valValue ){
            $res_str .=  $valKey ."->".$valValue."  " ;
        }
*/
        try {
            $locale = ut_loc_locale_compose( $value);
            $res_str .= "\n\nComposed Locale: ";
            if( $locale){
                $res_str .= "$locale";
            }else{
                $res_str .= "No values found from Locale compose due to the following error:\n";
                $res_str .= intl_get_error_message() ;
            }
        } catch (ValueError $exception) {
            echo $exception->getMessage() . "\n";
        }
    }

    $res_str .= "\n------------";
    $res_str .= "\n";
    return $res_str;

}

include_once( 'ut_common.inc' );
ut_run();

?>
--EXPECT--
Locale::composeLocale(): Argument #1 ($subtags) must contain a "language" key
locale_compose(): Argument #1 ($subtags) must contain a "language" key

------------
Input Array name is : loc1

Composed Locale: sl_Latn_IT
------------
Input Array name is : loc2

Composed Locale: de_DE
------------
Input Array name is : loc3

Composed Locale: hi
------------
Input Array name is : loc4

Composed Locale: zh_Hans_CN
------------
Input Array name is : loc5

Composed Locale: es_Hans_CN
------------
Input Array name is : loc6

Composed Locale: en_Hans_CN_nedis_rozaj
------------
Input Array name is : loc7

Composed Locale: en_lng_ing_Hans_CN_nedis_rozaj
------------
Input Array name is : loc8

Composed Locale: en_lng_ing_Hans_CN_nedis_rozaj_x_prv1_prv2
------------
Input Array name is : loc9
------------
Input Array name is : loc10

Composed Locale: No values found from Locale compose due to the following error:
locale_compose: parameter array element is not a string: U_ILLEGAL_ARGUMENT_ERROR
------------
Input Array name is : loc11

Composed Locale: No values found from Locale compose due to the following error:
locale_compose: parameter array element is not a string: U_ILLEGAL_ARGUMENT_ERROR
------------
Input Array name is : loc12

Composed Locale: en_lng_ing_Hans_CN_nedis_rozaj_x_prv1_prv2
------------
