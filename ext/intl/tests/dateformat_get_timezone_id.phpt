--TEST--
datefmt_get_timezone_id_code()
--INI--
date.timezone=Atlantic/Azores
intl.error_level=E_WARNING
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
		'US/Pacific',
		'US/Central'
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

Creating IntlDateFormatter with timezone_id = US/Pacific
After call to get_timezone_id :  timezone_id= US/Pacific

Creating IntlDateFormatter with timezone_id = US/Central
After call to get_timezone_id :  timezone_id= US/Central
