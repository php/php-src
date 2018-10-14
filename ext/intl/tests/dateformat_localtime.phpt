--TEST--
datefmt_localtime_code() icu <= 4.2
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
<?php if(version_compare(INTL_ICU_VERSION, '4.3', '<') != 1) print 'skip'; ?>
--INI--
date.timezone=UTC
--FILE--
<?php

/*
 * Test for the datefmt_localtime  function
 */


function ut_main()
{
	$locale_arr = array (
		'en_US_CA'
	);

	$datetype_arr = array (
                IntlDateFormatter::FULL,
                IntlDateFormatter::LONG,
                IntlDateFormatter::MEDIUM,
                IntlDateFormatter::SHORT,
                IntlDateFormatter::NONE
        );

        $res_str = '';

        $datetype_arr = array (
                IntlDateFormatter::FULL,
                IntlDateFormatter::LONG,
                IntlDateFormatter::MEDIUM,
        );

        $res_str = '';


        $text_arr = array (
                "Thursday, December 18, 1969 8:49:59 AM PST",
                "June 18, 1969 8:49:59 AM ",
                "12/18/69 8:49 AM",
                "19691218 08:49 AM"
        );

        $fmt1 = ut_datefmt_create( 'en_US_CA', IntlDateFormatter::LONG, IntlDateFormatter::LONG);
        $fmt2 = ut_datefmt_create( 'en_US_CA', IntlDateFormatter::MEDIUM, IntlDateFormatter::MEDIUM);
        $fmt3 = ut_datefmt_create( 'en_US_CA', IntlDateFormatter::FULL, IntlDateFormatter::FULL);
        $fmt_array  = array(
                $fmt1 , $fmt2 ,$fmt3
        );
        $fmt_desc_array  = array(
                "DateType::LONG, TimeType::LONG",
                "DateType::MEDIUM, TimeType::MEDIUM",
                "DateType::FULL, TimeType::FULL"
        );

	foreach( $text_arr as $text_entry){
                $res_str .= "\n-------------------------------\n";
                $res_str .= "\nInput text is : $text_entry";
		$cnt =0;


                    foreach( $fmt_array as $fmt_entry ){
			$res_str .= "\n------------";
			$res_str .= "\nIntlDateFormatter : ".$fmt_desc_array[$cnt];
		$parse_pos = 0;
			$cnt++;
			$parsed_arr = ut_datefmt_localtime( $fmt_entry , $text_entry , $parse_pos );

				if( $parsed_arr){
				    $res_str .= "\n";
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
		    }//end of for $fmt_array
        }


	return $res_str;

}

include_once( 'ut_common.inc' );

// Run the test
ut_run();
?>
--EXPECTF--
-------------------------------

Input text is : Thursday, December 18, 1969 8:49:59 AM PST
------------
IntlDateFormatter : DateType::LONG, TimeType::LONG
------------
IntlDateFormatter : DateType::MEDIUM, TimeType::MEDIUM
------------
IntlDateFormatter : DateType::FULL, TimeType::FULL
tm_sec : '59' , tm_min : '49' , tm_hour : '8' , tm_year : '69' , tm_mday : '18' , tm_wday : '4' , tm_yday : '352' , tm_mon : '11' , tm_isdst : '0' , 
-------------------------------

Input text is : June 18, 1969 8:49:59 AM 
------------
IntlDateFormatter : DateType::LONG, TimeType::LONG
------------
IntlDateFormatter : DateType::MEDIUM, TimeType::MEDIUM
tm_sec : '59' , tm_min : '49' , tm_hour : '8' , tm_year : '69' , tm_mday : '18' , tm_wday : '3' , tm_yday : '169' , tm_mon : '5' , tm_isdst : '%d' , 
------------
IntlDateFormatter : DateType::FULL, TimeType::FULL
-------------------------------

Input text is : 12/18/69 8:49 AM
------------
IntlDateFormatter : DateType::LONG, TimeType::LONG
------------
IntlDateFormatter : DateType::MEDIUM, TimeType::MEDIUM
------------
IntlDateFormatter : DateType::FULL, TimeType::FULL
-------------------------------

Input text is : 19691218 08:49 AM
------------
IntlDateFormatter : DateType::LONG, TimeType::LONG
------------
IntlDateFormatter : DateType::MEDIUM, TimeType::MEDIUM
------------
IntlDateFormatter : DateType::FULL, TimeType::FULL
