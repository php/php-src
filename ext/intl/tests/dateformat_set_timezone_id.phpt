--TEST--
datefmt_set_timezone_id_code()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

/*
 * Test for the datefmt_set_timezone_id  function
 */


function ut_main()
{
	$timezone_id_arr = array (
		'America/New_York',
		'America/Los_Angeles',
		'America/Chicago',
		'CN'
	);
	$timestamp_entry = 0;
	
	$res_str = '';

	$fmt = ut_datefmt_create( "en_US",  IntlDateFormatter::FULL, IntlDateFormatter::FULL, 'America/San_Francisco' , IntlDateFormatter::GREGORIAN  );
	$timezone_id = ut_datefmt_get_timezone_id( $fmt );
	$res_str .= "\nAfter creation of the dateformatter :  timezone_id= $timezone_id\n";

	foreach( $timezone_id_arr as $timezone_id_entry )
	{

		$res_str .= "-----------";
		$res_str .= "\nTrying to set timezone_id= $timezone_id_entry";
		ut_datefmt_set_timezone_id( $fmt , $timezone_id_entry );
		$timezone_id = ut_datefmt_get_timezone_id( $fmt );
		$res_str .= "\nAfter call to set_timezone_id :  timezone_id= $timezone_id";
		$formatted = ut_datefmt_format( $fmt, 0);
		$res_str .= "\nFormatting timestamp=0 resulted in  $formatted";
		$formatted = ut_datefmt_format( $fmt, 3600);
		$res_str .= "\nFormatting timestamp=3600 resulted in  $formatted";
		$res_str .= "\n";

	}

	return $res_str;

}

include_once( 'ut_common.inc' );

// Run the test
ut_run();
?>
--EXPECT--
After creation of the dateformatter :  timezone_id= America/San_Francisco
-----------
Trying to set timezone_id= America/New_York
After call to set_timezone_id :  timezone_id= America/New_York
Formatting timestamp=0 resulted in  Wednesday, December 31, 1969 7:00:00 PM ET
Formatting timestamp=3600 resulted in  Wednesday, December 31, 1969 8:00:00 PM ET
-----------
Trying to set timezone_id= America/Los_Angeles
After call to set_timezone_id :  timezone_id= America/Los_Angeles
Formatting timestamp=0 resulted in  Wednesday, December 31, 1969 4:00:00 PM PT
Formatting timestamp=3600 resulted in  Wednesday, December 31, 1969 5:00:00 PM PT
-----------
Trying to set timezone_id= America/Chicago
After call to set_timezone_id :  timezone_id= America/Chicago
Formatting timestamp=0 resulted in  Wednesday, December 31, 1969 6:00:00 PM CT
Formatting timestamp=3600 resulted in  Wednesday, December 31, 1969 7:00:00 PM CT
-----------
Trying to set timezone_id= CN
After call to set_timezone_id :  timezone_id= CN
Formatting timestamp=0 resulted in  Thursday, January 1, 1970 12:00:00 AM GMT+00:00
Formatting timestamp=3600 resulted in  Thursday, January 1, 1970 1:00:00 AM GMT+00:00