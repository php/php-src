--TEST--
datefmt_format_code() and datefmt_parse_code()
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '51.2') < 0) die('skip for ICU >= 51.2'); ?>
<?php if (version_compare(INTL_ICU_VERSION, '72.1') >= 0) die('skip for ICU < 72.1'); ?>
--FILE--
<?php

/*
 * Test for the datefmt_format  function
 */


function ut_main()
{
    $timezone = 'GMT+05:00';

    $locale_arr = array (
        'en_US'
    );

    $datetype_arr = array (
                IntlDateFormatter::FULL,
                IntlDateFormatter::LONG,
                IntlDateFormatter::MEDIUM
        );

        $res_str = '';


    $time_arr = array (
        0,
        -1200000,
        1200000,
        2200000000,
        -2200000000,
        90099999,
        3600,
        -3600
    );

    $localtime_arr1 = array (
        'tm_sec' => 24 ,
        'tm_min' => 3,
        'tm_hour' => 19,
        'tm_mday' => 3,
        'tm_mon' => 3,
        'tm_year' => 105,
    );
    $localtime_arr2 = array (
        'tm_sec' => 21,
        'tm_min' => 5,
        'tm_hour' => 7,
        'tm_mday' => 13,
        'tm_mon' => 7,
        'tm_year' => 205,
    );
    $localtime_arr3 = array (
            'tm_sec' => 11,
            'tm_min' => 13,
            'tm_hour' => 0,
            'tm_mday' => 17,
            'tm_mon' => 11,
            'tm_year' => -5
        );

    $localtime_arr = array (
        $localtime_arr1,
        $localtime_arr2,
        $localtime_arr3
    );

    //Test format and parse with a timestamp : long
    foreach( $time_arr as $timestamp_entry){
        $res_str .= "\n------------\n";
        $res_str .= "\nInput timestamp is : $timestamp_entry";
        $res_str .= "\n------------\n";
        foreach( $locale_arr as $locale_entry ){
            foreach( $datetype_arr as $datetype_entry ) {
                $res_str .= "\nIntlDateFormatter locale= $locale_entry ,datetype = $datetype_entry ,timetype =$datetype_entry ";
                $fmt = ut_datefmt_create( $locale_entry , $datetype_entry ,$datetype_entry,$timezone);
                $formatted = ut_datefmt_format( $fmt , $timestamp_entry);
                $res_str .= "\nFormatted timestamp is : $formatted";
                $parsed = ut_datefmt_parse( $fmt , $formatted);
                if( intl_get_error_code() == U_ZERO_ERROR){
                    $res_str .= "\nParsed timestamp is : $parsed";
                }else{
                    $res_str .= "\nError while parsing as: '".intl_get_error_message()."'";
                }
            }
        }
    }

    //Test format and parse with a localtime :array
    foreach( $localtime_arr as $localtime_entry){
        $res_str .= "\n------------\n";
        $res_str .= "\nInput localtime is : ";
        foreach( $localtime_entry as $key => $value){
                    $res_str .= "$key : '$value' , ";
        }

        $res_str .= "\n------------\n";
        foreach( $locale_arr as $locale_entry ){
            foreach( $datetype_arr as $datetype_entry ) {
                $res_str .= "\nIntlDateFormatter locale= $locale_entry ,datetype = $datetype_entry ,timetype =$datetype_entry ";
                $fmt = ut_datefmt_create( $locale_entry , $datetype_entry ,$datetype_entry,$timezone);
                $formatted1 = ut_datefmt_format( $fmt , $localtime_entry);
                if( intl_get_error_code() == U_ZERO_ERROR){
                    $res_str .= "\nFormatted localtime_array is : $formatted1";
                }else{
                    $res_str .= "\nError while formatting as: '".intl_get_error_message()."'";
                }
                //Parsing
                $parsed_arr = ut_datefmt_localtime( $fmt, $formatted1 );

                if( $parsed_arr){
                    $res_str .= "\nParsed array is: ";
                    foreach( $parsed_arr as $key => $value){
                        $res_str .= "$key : '$value' , ";
                    }
                }
/*
                else{
                    //$res_str .= "No values found from LocaleTime parsing.";
                    $res_str .= "\tError : '".intl_get_error_message()."'";
                }
*/
            }
        }
    }

    return $res_str;

}

include_once( 'ut_common.inc' );

// Run the test
ut_run();
?>
--EXPECT--
------------

Input timestamp is : 0
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0 
Formatted timestamp is : Thursday, January 1, 1970 at 5:00:00 AM GMT+05:00
Parsed timestamp is : 0
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1 
Formatted timestamp is : January 1, 1970 at 5:00:00 AM GMT+5
Parsed timestamp is : 0
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2 
Formatted timestamp is : Jan 1, 1970, 5:00:00 AM
Parsed timestamp is : 0
------------

Input timestamp is : -1200000
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0 
Formatted timestamp is : Thursday, December 18, 1969 at 7:40:00 AM GMT+05:00
Parsed timestamp is : -1200000
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1 
Formatted timestamp is : December 18, 1969 at 7:40:00 AM GMT+5
Parsed timestamp is : -1200000
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2 
Formatted timestamp is : Dec 18, 1969, 7:40:00 AM
Parsed timestamp is : -1200000
------------

Input timestamp is : 1200000
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0 
Formatted timestamp is : Thursday, January 15, 1970 at 2:20:00 AM GMT+05:00
Parsed timestamp is : 1200000
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1 
Formatted timestamp is : January 15, 1970 at 2:20:00 AM GMT+5
Parsed timestamp is : 1200000
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2 
Formatted timestamp is : Jan 15, 1970, 2:20:00 AM
Parsed timestamp is : 1200000
------------

Input timestamp is : 2200000000
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0 
Formatted timestamp is : Monday, September 19, 2039 at 4:06:40 AM GMT+05:00
Parsed timestamp is : 2200000000
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1 
Formatted timestamp is : September 19, 2039 at 4:06:40 AM GMT+5
Parsed timestamp is : 2200000000
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2 
Formatted timestamp is : Sep 19, 2039, 4:06:40 AM
Parsed timestamp is : 2200000000
------------

Input timestamp is : -2200000000
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0 
Formatted timestamp is : Sunday, April 15, 1900 at 5:53:20 AM GMT+05:00
Parsed timestamp is : -2200000000
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1 
Formatted timestamp is : April 15, 1900 at 5:53:20 AM GMT+5
Parsed timestamp is : -2200000000
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2 
Formatted timestamp is : Apr 15, 1900, 5:53:20 AM
Parsed timestamp is : -2200000000
------------

Input timestamp is : 90099999
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0 
Formatted timestamp is : Thursday, November 9, 1972 at 12:46:39 AM GMT+05:00
Parsed timestamp is : 90099999
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1 
Formatted timestamp is : November 9, 1972 at 12:46:39 AM GMT+5
Parsed timestamp is : 90099999
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2 
Formatted timestamp is : Nov 9, 1972, 12:46:39 AM
Parsed timestamp is : 90099999
------------

Input timestamp is : 3600
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0 
Formatted timestamp is : Thursday, January 1, 1970 at 6:00:00 AM GMT+05:00
Parsed timestamp is : 3600
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1 
Formatted timestamp is : January 1, 1970 at 6:00:00 AM GMT+5
Parsed timestamp is : 3600
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2 
Formatted timestamp is : Jan 1, 1970, 6:00:00 AM
Parsed timestamp is : 3600
------------

Input timestamp is : -3600
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0 
Formatted timestamp is : Thursday, January 1, 1970 at 4:00:00 AM GMT+05:00
Parsed timestamp is : -3600
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1 
Formatted timestamp is : January 1, 1970 at 4:00:00 AM GMT+5
Parsed timestamp is : -3600
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2 
Formatted timestamp is : Jan 1, 1970, 4:00:00 AM
Parsed timestamp is : -3600
------------

Input localtime is : tm_sec : '24' , tm_min : '3' , tm_hour : '19' , tm_mday : '3' , tm_mon : '3' , tm_year : '105' , 
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0 
Formatted localtime_array is : Sunday, April 3, 2005 at 7:03:24 PM GMT+05:00
Parsed array is: tm_sec : '24' , tm_min : '3' , tm_hour : '19' , tm_year : '105' , tm_mday : '3' , tm_wday : '0' , tm_yday : '93' , tm_mon : '3' , tm_isdst : '0' , 
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1 
Formatted localtime_array is : April 3, 2005 at 7:03:24 PM GMT+5
Parsed array is: tm_sec : '24' , tm_min : '3' , tm_hour : '19' , tm_year : '105' , tm_mday : '3' , tm_wday : '0' , tm_yday : '93' , tm_mon : '3' , tm_isdst : '0' , 
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2 
Formatted localtime_array is : Apr 3, 2005, 7:03:24 PM
Parsed array is: tm_sec : '24' , tm_min : '3' , tm_hour : '19' , tm_year : '105' , tm_mday : '3' , tm_wday : '0' , tm_yday : '93' , tm_mon : '3' , tm_isdst : '0' , 
------------

Input localtime is : tm_sec : '21' , tm_min : '5' , tm_hour : '7' , tm_mday : '13' , tm_mon : '7' , tm_year : '205' , 
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0 
Formatted localtime_array is : Thursday, August 13, 2105 at 7:05:21 AM GMT+05:00
Parsed array is: tm_sec : '21' , tm_min : '5' , tm_hour : '7' , tm_year : '205' , tm_mday : '13' , tm_wday : '4' , tm_yday : '225' , tm_mon : '7' , tm_isdst : '0' , 
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1 
Formatted localtime_array is : August 13, 2105 at 7:05:21 AM GMT+5
Parsed array is: tm_sec : '21' , tm_min : '5' , tm_hour : '7' , tm_year : '205' , tm_mday : '13' , tm_wday : '4' , tm_yday : '225' , tm_mon : '7' , tm_isdst : '0' , 
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2 
Formatted localtime_array is : Aug 13, 2105, 7:05:21 AM
Parsed array is: tm_sec : '21' , tm_min : '5' , tm_hour : '7' , tm_year : '205' , tm_mday : '13' , tm_wday : '4' , tm_yday : '225' , tm_mon : '7' , tm_isdst : '0' , 
------------

Input localtime is : tm_sec : '11' , tm_min : '13' , tm_hour : '0' , tm_mday : '17' , tm_mon : '11' , tm_year : '-5' , 
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0 
Formatted localtime_array is : Tuesday, December 17, 1895 at 12:13:11 AM GMT+05:00
Parsed array is: tm_sec : '11' , tm_min : '13' , tm_hour : '0' , tm_year : '-5' , tm_mday : '17' , tm_wday : '2' , tm_yday : '351' , tm_mon : '11' , tm_isdst : '0' , 
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1 
Formatted localtime_array is : December 17, 1895 at 12:13:11 AM GMT+5
Parsed array is: tm_sec : '11' , tm_min : '13' , tm_hour : '0' , tm_year : '-5' , tm_mday : '17' , tm_wday : '2' , tm_yday : '351' , tm_mon : '11' , tm_isdst : '0' , 
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2 
Formatted localtime_array is : Dec 17, 1895, 12:13:11 AM
Parsed array is: tm_sec : '11' , tm_min : '13' , tm_hour : '0' , tm_year : '-5' , tm_mday : '17' , tm_wday : '2' , tm_yday : '351' , tm_mon : '11' , tm_isdst : '0' ,
