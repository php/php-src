--TEST--
datefmt_parse_timestamp_code()  with parse pos
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

/*
 * Test for the datefmt_parse_timestamp  function with parse_pos
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


	$text_arr = array (
		"Sunday, September 18, 3039 4:06:40 PM PT",
		"Thursday, December 18, 1969 8:49:59 AM PST",
		//"December 18, 1969 8:49:59 AM PST",
		//"12/18/69 8:49 AM",
		"19001218 08:49 AM",
		"19691218 08:49 AM"
	);

	foreach( $text_arr as $text_entry){
                $res_str .= "\n------------\n";
                $res_str .= "\nInput text is : $text_entry";
                $res_str .= "\n------------";

                foreach( $locale_arr as $locale_entry ){
			$res_str .= "\nLocale is : $locale_entry";
			$res_str .= "\n------------";
                        foreach( $datetype_arr as $datetype_entry )
			{
				$res_str .= "\ndatetype = $datetype_entry ,timetype =$datetype_entry ";
				$fmt = ut_datefmt_create( $locale_entry , $datetype_entry ,$datetype_entry);
				$parsed = ut_datefmt_parse( $fmt , $text_entry);
				if( intl_get_error_code() == U_ZERO_ERROR){
					$res_str .= "\nParsed text is : $parsed";
				}else{
					$res_str .= "\nError while parsing as: '".intl_get_error_message()."'";
				}
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

Input text is : Sunday, September 18, 3039 4:06:40 PM PT
------------
Locale is : en_US_CA
------------
datetype = 0 ,timetype =0 
Error while parsing as: 'datefmt_parse: parsing of input parametrs resulted in value larger than data type long can handle.
The valid range of a timestamp is typically from Fri, 13 Dec 1901 20:45:54 GMT to Tue, 19 Jan 2038 03:14:07 GMT.: U_BUFFER_OVERFLOW_ERROR'
datetype = 1 ,timetype =1 
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'
datetype = 2 ,timetype =2 
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'
datetype = 3 ,timetype =3 
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'
datetype = -1 ,timetype =-1 
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'
------------

Input text is : Thursday, December 18, 1969 8:49:59 AM PST
------------
Locale is : en_US_CA
------------
datetype = 0 ,timetype =0 
Parsed text is : -1149001
datetype = 1 ,timetype =1 
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'
datetype = 2 ,timetype =2 
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'
datetype = 3 ,timetype =3 
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'
datetype = -1 ,timetype =-1 
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'
------------

Input text is : 19001218 08:49 AM
------------
Locale is : en_US_CA
------------
datetype = 0 ,timetype =0 
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'
datetype = 1 ,timetype =1 
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'
datetype = 2 ,timetype =2 
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'
datetype = 3 ,timetype =3 
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'
datetype = -1 ,timetype =-1 
Error while parsing as: 'datefmt_parse: parsing of input parametrs resulted in value larger than data type long can handle.
The valid range of a timestamp is typically from Fri, 13 Dec 1901 20:45:54 GMT to Tue, 19 Jan 2038 03:14:07 GMT.: U_BUFFER_OVERFLOW_ERROR'
------------

Input text is : 19691218 08:49 AM
------------
Locale is : en_US_CA
------------
datetype = 0 ,timetype =0 
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'
datetype = 1 ,timetype =1 
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'
datetype = 2 ,timetype =2 
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'
datetype = 3 ,timetype =3 
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'
datetype = -1 ,timetype =-1 
Parsed text is : -1149060
