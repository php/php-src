--TEST--
datefmt_parse_code() icu <= 4.2
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
<?php if(version_compare(INTL_ICU_VERSION, '4.3', '<') != 1) print 'skip'; ?>
--INI--
date.timezone="America/Los_Angeles"
--FILE--
<?php

/*
 * Test for the datefmt_parse  function
 */

putenv('TZ=America/Los_Angeles');

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
		// Full parsing
		array("Sunday, September 18, 2039 4:06:40 PM PT", IntlDateFormatter::FULL, IntlDateFormatter::FULL),
		array("Wednesday, December 17, 1969 6:40:00 PM PT", IntlDateFormatter::FULL, IntlDateFormatter::FULL),
		array("Thursday, December 18, 1969 8:49:59 PM PST", IntlDateFormatter::FULL, IntlDateFormatter::FULL),
		array("December 18, 1969 8:49:59 AM PST", IntlDateFormatter::LONG, IntlDateFormatter::FULL),
		array("12/18/69 8:49 AM", IntlDateFormatter::SHORT, IntlDateFormatter::SHORT),
		array("19691218 08:49 AM", IntlDateFormatter::SHORT, IntlDateFormatter::SHORT),
		// Partial parsing
		array("Sunday, September 18, 2039 4:06:40 PM PT", IntlDateFormatter::FULL, IntlDateFormatter::NONE),
		array("Sunday, September 18, 2039 4:06:40 PM PT", IntlDateFormatter::FULL, IntlDateFormatter::SHORT),
		array("December 18, 1969 8:49:59 AM PST", IntlDateFormatter::LONG, IntlDateFormatter::NONE),
		array("December 18, 1969 8:49:59 AM PST", IntlDateFormatter::LONG, IntlDateFormatter::SHORT),
		array("12/18/69 8:49 AM", IntlDateFormatter::SHORT, IntlDateFormatter::LONG),
		array("19691218 08:49 AM", IntlDateFormatter::SHORT, IntlDateFormatter::LONG),
	);

	foreach( $text_arr as $text_entry){
		$fmt = ut_datefmt_create( 'en_US_CA', $text_entry[1], $text_entry[2]);
		$parse_pos = 0;
		$parsed = ut_datefmt_parse( $fmt , $text_entry[0] , $parse_pos );

		$res_str .= "\nInput text : {$text_entry[0]} ; DF = {$text_entry[1]}; TF = {$text_entry[2]}";
		if( intl_get_error_code() != U_ZERO_ERROR) {
			$res_str .= "\nError : ".intl_get_error_message();
		}
		$res_str .= "\nParsed: $parsed; parse_pos : $parse_pos\n";
	}

	return $res_str;

}

include_once( 'ut_common.inc' );

// Run the test
ut_run();
?>
--EXPECT--
Input text : Sunday, September 18, 2039 4:06:40 PM PT ; DF = 0; TF = 0
Parsed: 2200000000; parse_pos : 40

Input text : Wednesday, December 17, 1969 6:40:00 PM PT ; DF = 0; TF = 0
Parsed: -1200000; parse_pos : 42

Input text : Thursday, December 18, 1969 8:49:59 PM PST ; DF = 0; TF = 0
Parsed: -1105801; parse_pos : 42

Input text : December 18, 1969 8:49:59 AM PST ; DF = 1; TF = 0
Parsed: -1149001; parse_pos : 32

Input text : 12/18/69 8:49 AM ; DF = 3; TF = 3
Parsed: -1149060; parse_pos : 16

Input text : 19691218 08:49 AM ; DF = 3; TF = 3
Error : Date parsing failed: U_PARSE_ERROR
Parsed: ; parse_pos : 8

Input text : Sunday, September 18, 2039 4:06:40 PM PT ; DF = 0; TF = -1
Parsed: 2199942000; parse_pos : 26

Input text : Sunday, September 18, 2039 4:06:40 PM PT ; DF = 0; TF = 3
Error : Date parsing failed: U_PARSE_ERROR
Parsed: ; parse_pos : 31

Input text : December 18, 1969 8:49:59 AM PST ; DF = 1; TF = -1
Parsed: -1180800; parse_pos : 17

Input text : December 18, 1969 8:49:59 AM PST ; DF = 1; TF = 3
Error : Date parsing failed: U_PARSE_ERROR
Parsed: ; parse_pos : 22

Input text : 12/18/69 8:49 AM ; DF = 3; TF = 1
Error : Date parsing failed: U_PARSE_ERROR
Parsed: ; parse_pos : 13

Input text : 19691218 08:49 AM ; DF = 3; TF = 1
Error : Date parsing failed: U_PARSE_ERROR
Parsed: ; parse_pos : 8
