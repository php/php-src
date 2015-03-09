--TEST--
datefmt_parse_timestamp_code()  with parse pos icu <= 4.2
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
<?php if(version_compare(INTL_ICU_VERSION, '4.3', '<') != 1) print 'skip'; ?>
--INI--
date.timezone="America/Los_Angeles"
--ENV--
TZ=America/Los_Angeles
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
		"12/18/69 8:49 AM",
		"20111218 08:49 AM",
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
				$res_str .= "\ndatetype = $datetype_entry ,timetype =$datetype_entry";
				$fmt = ut_datefmt_create( $locale_entry , $datetype_entry ,$datetype_entry);
				$pos = 0;
				$parsed = ut_datefmt_parse( $fmt , $text_entry, $pos);
				if( intl_get_error_code() == U_ZERO_ERROR){
					$res_str .= "\nParsed text is : $parsed; Position = $pos";
				}else{
					$res_str .= "\nError while parsing as: '".intl_get_error_message()."'; Position = $pos";
				}
			}
		}
        }
	$res_str .= "\n";

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
Parsed text is : 33756908800; Position = 40
datetype = 1 ,timetype =1
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'; Position = 0
datetype = 2 ,timetype =2
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'; Position = 0
datetype = 3 ,timetype =3
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'; Position = 0
datetype = -1 ,timetype =-1
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'; Position = 0
------------

Input text is : Thursday, December 18, 1969 8:49:59 AM PST
------------
Locale is : en_US_CA
------------
datetype = 0 ,timetype =0
Parsed text is : -1149001; Position = 42
datetype = 1 ,timetype =1
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'; Position = 0
datetype = 2 ,timetype =2
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'; Position = 0
datetype = 3 ,timetype =3
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'; Position = 0
datetype = -1 ,timetype =-1
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'; Position = 0
------------

Input text is : 12/18/69 8:49 AM
------------
Locale is : en_US_CA
------------
datetype = 0 ,timetype =0
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'; Position = 0
datetype = 1 ,timetype =1
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'; Position = 0
datetype = 2 ,timetype =2
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'; Position = 0
datetype = 3 ,timetype =3
Parsed text is : -1149060; Position = 16
datetype = -1 ,timetype =-1
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'; Position = 0
------------

Input text is : 20111218 08:49 AM
------------
Locale is : en_US_CA
------------
datetype = 0 ,timetype =0
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'; Position = 0
datetype = 1 ,timetype =1
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'; Position = 0
datetype = 2 ,timetype =2
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'; Position = 0
datetype = 3 ,timetype =3
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'; Position = 8
datetype = -1 ,timetype =-1
Parsed text is : 1324226940; Position = 17
------------

Input text is : 19691218 08:49 AM
------------
Locale is : en_US_CA
------------
datetype = 0 ,timetype =0
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'; Position = 0
datetype = 1 ,timetype =1
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'; Position = 0
datetype = 2 ,timetype =2
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'; Position = 0
datetype = 3 ,timetype =3
Error while parsing as: 'Date parsing failed: U_PARSE_ERROR'; Position = 8
datetype = -1 ,timetype =-1
Parsed text is : -1149060; Position = 17
