--TEST--
datefmt_get_timezone_id_code()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

/*
 * Test for the datefmt_get_timezone_id  function
 */


function ut_main()
{
	$timezone_id_arr = array (
		'America/New_York',
		'America/Los_Angeles',
		'America/Dallas'
	);
	
	$res_str = '';

	foreach( $timezone_id_arr as $timezone_id_entry )
	{
		$res_str .= "\nCreating IntlDateFormatter with timezone_id = $timezone_id_entry";
		$fmt = ut_datefmt_create( "de-DE",  IntlDateFormatter::SHORT, IntlDateFormatter::SHORT, $timezone_id_entry , IntlDateFormatter::GREGORIAN  );
		$timezone_id = ut_datefmt_get_timezone_id( $fmt);
		$res_str .= "\nAfter call to get_timezone_id :  timezone_id= $timezone_id";
		$res_str .= "\n";
	}

	return $res_str;

}

include_once( 'ut_common.inc' );

// Run the test
ut_run();
?>
--EXPECT--
Creating IntlDateFormatter with timezone_id = America/New_York
After call to get_timezone_id :  timezone_id= America/New_York

Creating IntlDateFormatter with timezone_id = America/Los_Angeles
After call to get_timezone_id :  timezone_id= America/Los_Angeles

Creating IntlDateFormatter with timezone_id = America/Dallas
After call to get_timezone_id :  timezone_id= America/Dallas
