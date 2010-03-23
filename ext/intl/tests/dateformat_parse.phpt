--TEST--
datefmt_parse_code()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

/*
 * Test for the datefmt_parse  function
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
		"Sunday, September 18, 2039 4:06:40 PM PT",
		"Wednesday, December 17, 1969 6:40:00 PM PT",
		"Thursday, December 18, 1969 8:49:59 AM PST",
		"Thursday, December 18, 1969 8:49:59 PM PST",
		"December 18, 1969 8:49:59 AM PST",
		"12/18/69 8:49 AM",
		"19691218 08:49 AM"
	);

	$parse_pos = 0;
				$fmt = ut_datefmt_create( 'en_US_CA', IntlDateFormatter::NONE, IntlDateFormatter::SHORT);
	foreach( $text_arr as $text_entry){
		for ( $parse_pos = 0 ; $parse_pos< strlen($text_entry) ; $parse_pos++ ){
				$i = $parse_pos;
				$parsed = ut_datefmt_parse( $fmt , $text_entry , $parse_pos );
				if( intl_get_error_code() == U_ZERO_ERROR){
					$res_str .= "\nInput text :$text_entry ; Parsed text : $parsed";
					$res_str .= " ; parse_pos : $i $parse_pos";
				} else {
					$parse_pos = $i;
				}
/*
				else{
					$res_str .= "\nError while parsing as: '".intl_get_error_message()."'";
				}
*/
		}
	}


	return $res_str;

}

include_once( 'ut_common.inc' );

// Run the test
ut_run();
?>
--EXPECT--
Input text :Sunday, September 18, 2039 4:06:40 PM PT ; Parsed text : 96000 ; parse_pos : 29 37
Input text :Wednesday, December 17, 1969 6:40:00 PM PT ; Parsed text : 216000 ; parse_pos : 31 39
Input text :Thursday, December 18, 1969 8:49:59 AM PST ; Parsed text : 208740 ; parse_pos : 30 38
Input text :Thursday, December 18, 1969 8:49:59 PM PST ; Parsed text : 251940 ; parse_pos : 30 38
Input text :December 18, 1969 8:49:59 AM PST ; Parsed text : 208740 ; parse_pos : 20 28
Input text :12/18/69 8:49 AM ; Parsed text : 60540 ; parse_pos : 8 16
Input text :19691218 08:49 AM ; Parsed text : 60540 ; parse_pos : 8 17
