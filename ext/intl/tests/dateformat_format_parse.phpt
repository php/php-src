--TEST--
datefmt_format_code() and datefmt_parse_code()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
<?php print 'skip'; ?>
--FILE--
<?php

/*
 * Test for the datefmt_format  function
 */


function ut_main()
{
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
	    'tm_wday' => 0,
	    'tm_yday' => 93,
	    'tm_isdst' => 1
	);
	$localtime_arr2 = array (
	    'tm_sec' => 24 ,
	    'tm_min' => 3,
	    'tm_hour' => 3,
	    'tm_mday' => 3,
	    'tm_mon' => 3,
	    'tm_year' => 205,
	    'tm_wday' => 5,
	    'tm_yday' => 93,
	    'tm_isdst' => 1
	);
	$localtime_arr3 = array (
            'tm_sec' => 24 ,
            'tm_min' => 3,
            'tm_hour' => 3,
            'tm_mday' => 3,
            'tm_mon' => 3,
            'tm_year' => -5,
            'tm_wday' => 3,
            'tm_yday' => 93,
            'tm_isdst' => 1
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
				$fmt = ut_datefmt_create( $locale_entry , $datetype_entry ,$datetype_entry,'America/Los_Angeles', IntlDateFormatter::GREGORIAN  );
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
				$fmt = ut_datefmt_create( $locale_entry , $datetype_entry ,$datetype_entry);
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
Formatted timestamp is : Wednesday, December 31, 1969 4:00:00 PM PT
Parsed timestamp is : 0
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1 
Formatted timestamp is : December 31, 1969 4:00:00 PM PST
Parsed timestamp is : 0
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2 
Formatted timestamp is : Dec 31, 1969 4:00:00 PM
Parsed timestamp is : 0
------------

Input timestamp is : -1200000
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0 
Formatted timestamp is : Wednesday, December 17, 1969 6:40:00 PM PT
Parsed timestamp is : -1200000
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1 
Formatted timestamp is : December 17, 1969 6:40:00 PM PST
Parsed timestamp is : -1200000
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2 
Formatted timestamp is : Dec 17, 1969 6:40:00 PM
Parsed timestamp is : -1200000
------------

Input timestamp is : 1200000
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0 
Formatted timestamp is : Wednesday, January 14, 1970 1:20:00 PM PT
Parsed timestamp is : 1200000
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1 
Formatted timestamp is : January 14, 1970 1:20:00 PM PST
Parsed timestamp is : 1200000
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2 
Formatted timestamp is : Jan 14, 1970 1:20:00 PM
Parsed timestamp is : 1200000
------------

Input timestamp is : 2200000000
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0 
Formatted timestamp is : Sunday, September 18, 2039 4:06:40 PM PT
Error while parsing as: 'datefmt_parse: parsing of input parametrs resulted in value larger than data type long can handle.
The valid range of a timestamp is typically from Fri, 13 Dec 1901 20:45:54 GMT to Tue, 19 Jan 2038 03:14:07 GMT.: U_BUFFER_OVERFLOW_ERROR'
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1 
Formatted timestamp is : September 18, 2039 4:06:40 PM PDT
Error while parsing as: 'datefmt_parse: parsing of input parametrs resulted in value larger than data type long can handle.
The valid range of a timestamp is typically from Fri, 13 Dec 1901 20:45:54 GMT to Tue, 19 Jan 2038 03:14:07 GMT.: U_BUFFER_OVERFLOW_ERROR'
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2 
Formatted timestamp is : Sep 18, 2039 4:06:40 PM
Error while parsing as: 'datefmt_parse: parsing of input parametrs resulted in value larger than data type long can handle.
The valid range of a timestamp is typically from Fri, 13 Dec 1901 20:45:54 GMT to Tue, 19 Jan 2038 03:14:07 GMT.: U_BUFFER_OVERFLOW_ERROR'
------------

Input timestamp is : -2200000000
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0 
Formatted timestamp is : Saturday, April 14, 1900 5:53:20 PM PT
Error while parsing as: 'datefmt_parse: parsing of input parametrs resulted in value larger than data type long can handle.
The valid range of a timestamp is typically from Fri, 13 Dec 1901 20:45:54 GMT to Tue, 19 Jan 2038 03:14:07 GMT.: U_BUFFER_OVERFLOW_ERROR'
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1 
Formatted timestamp is : April 14, 1900 5:53:20 PM PDT
Error while parsing as: 'datefmt_parse: parsing of input parametrs resulted in value larger than data type long can handle.
The valid range of a timestamp is typically from Fri, 13 Dec 1901 20:45:54 GMT to Tue, 19 Jan 2038 03:14:07 GMT.: U_BUFFER_OVERFLOW_ERROR'
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2 
Formatted timestamp is : Apr 14, 1900 5:53:20 PM
Error while parsing as: 'datefmt_parse: parsing of input parametrs resulted in value larger than data type long can handle.
The valid range of a timestamp is typically from Fri, 13 Dec 1901 20:45:54 GMT to Tue, 19 Jan 2038 03:14:07 GMT.: U_BUFFER_OVERFLOW_ERROR'
------------

Input timestamp is : 90099999
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0 
Formatted timestamp is : Wednesday, November 8, 1972 11:46:39 AM PT
Parsed timestamp is : 90099999
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1 
Formatted timestamp is : November 8, 1972 11:46:39 AM PST
Parsed timestamp is : 90099999
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2 
Formatted timestamp is : Nov 8, 1972 11:46:39 AM
Parsed timestamp is : 90099999
------------

Input timestamp is : 3600
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0 
Formatted timestamp is : Wednesday, December 31, 1969 5:00:00 PM PT
Parsed timestamp is : 3600
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1 
Formatted timestamp is : December 31, 1969 5:00:00 PM PST
Parsed timestamp is : 3600
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2 
Formatted timestamp is : Dec 31, 1969 5:00:00 PM
Parsed timestamp is : 3600
------------

Input timestamp is : -3600
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0 
Formatted timestamp is : Wednesday, December 31, 1969 3:00:00 PM PT
Parsed timestamp is : -3600
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1 
Formatted timestamp is : December 31, 1969 3:00:00 PM PST
Parsed timestamp is : -3600
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2 
Formatted timestamp is : Dec 31, 1969 3:00:00 PM
Parsed timestamp is : -3600
------------

Input localtime is : tm_sec : '24' , tm_min : '3' , tm_hour : '19' , tm_mday : '3' , tm_mon : '3' , tm_year : '105' , tm_wday : '0' , tm_yday : '93' , tm_isdst : '1' , 
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0 
Formatted localtime_array is : Sunday, April 3, 2005 7:03:24 PM PT
Parsed array is: tm_sec : '24' , tm_min : '3' , tm_hour : '19' , tm_year : '105' , tm_mday : '3' , tm_wday : '0' , tm_yday : '93' , tm_mon : '3' , tm_isdst : '1' , 
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1 
Formatted localtime_array is : April 3, 2005 7:03:24 PM PDT
Parsed array is: tm_sec : '24' , tm_min : '3' , tm_hour : '19' , tm_year : '105' , tm_mday : '3' , tm_wday : '0' , tm_yday : '93' , tm_mon : '3' , tm_isdst : '1' , 
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2 
Formatted localtime_array is : Apr 3, 2005 7:03:24 PM
Parsed array is: tm_sec : '24' , tm_min : '3' , tm_hour : '19' , tm_year : '105' , tm_mday : '3' , tm_wday : '0' , tm_yday : '93' , tm_mon : '3' , tm_isdst : '1' , 
------------

Input localtime is : tm_sec : '24' , tm_min : '3' , tm_hour : '3' , tm_mday : '3' , tm_mon : '3' , tm_year : '205' , tm_wday : '5' , tm_yday : '93' , tm_isdst : '1' , 
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0 
Formatted localtime_array is : Friday, April 3, 2105 3:03:24 AM PT
Parsed array is: tm_sec : '24' , tm_min : '3' , tm_hour : '3' , tm_year : '205' , tm_mday : '3' , tm_wday : '5' , tm_yday : '93' , tm_mon : '3' , tm_isdst : '1' , 
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1 
Formatted localtime_array is : April 3, 2105 3:03:24 AM PDT
Parsed array is: tm_sec : '24' , tm_min : '3' , tm_hour : '3' , tm_year : '205' , tm_mday : '3' , tm_wday : '5' , tm_yday : '93' , tm_mon : '3' , tm_isdst : '1' , 
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2 
Formatted localtime_array is : Apr 3, 2105 3:03:24 AM
Parsed array is: tm_sec : '24' , tm_min : '3' , tm_hour : '3' , tm_year : '205' , tm_mday : '3' , tm_wday : '5' , tm_yday : '93' , tm_mon : '3' , tm_isdst : '1' , 
------------

Input localtime is : tm_sec : '24' , tm_min : '3' , tm_hour : '3' , tm_mday : '3' , tm_mon : '3' , tm_year : '-5' , tm_wday : '3' , tm_yday : '93' , tm_isdst : '1' , 
------------

IntlDateFormatter locale= en_US ,datetype = 0 ,timetype =0 
Formatted localtime_array is : Wednesday, April 3, 1895 3:03:24 AM PT
Parsed array is: tm_sec : '24' , tm_min : '3' , tm_hour : '3' , tm_year : '-5' , tm_mday : '3' , tm_wday : '3' , tm_yday : '93' , tm_mon : '3' , tm_isdst : '1' , 
IntlDateFormatter locale= en_US ,datetype = 1 ,timetype =1 
Formatted localtime_array is : April 3, 1895 3:03:24 AM PDT
Parsed array is: tm_sec : '24' , tm_min : '3' , tm_hour : '3' , tm_year : '-5' , tm_mday : '3' , tm_wday : '3' , tm_yday : '93' , tm_mon : '3' , tm_isdst : '1' , 
IntlDateFormatter locale= en_US ,datetype = 2 ,timetype =2 
Formatted localtime_array is : Apr 3, 1895 3:03:24 AM
Parsed array is: tm_sec : '24' , tm_min : '3' , tm_hour : '3' , tm_year : '-5' , tm_mday : '3' , tm_wday : '3' , tm_yday : '93' , tm_mon : '3' , tm_isdst : '1' ,
