--TEST--
datefmt_get_timetype_code()
--EXTENSIONS--
intl
--FILE--
<?php

/*
 * Test for the datefmt_get_timetype  function
 */


function ut_main()
{
    $timetype_arr = array (
        IntlDateFormatter::FULL,
        IntlDateFormatter::LONG,
        IntlDateFormatter::MEDIUM,
        IntlDateFormatter::SHORT,
        IntlDateFormatter::NONE
    );

    $res_str = '';

    foreach( $timetype_arr as $timetype_entry )
    {
        $res_str .= "\nCreating IntlDateFormatter with time_type = $timetype_entry";
        $fmt = ut_datefmt_create( "de-DE",  IntlDateFormatter::SHORT, $timetype_entry ,'America/Los_Angeles', IntlDateFormatter::GREGORIAN  );
        $time_type = ut_datefmt_get_timetype( $fmt);
        $res_str .= "\nAfter call to get_timetype :  timetype= $time_type";
        $res_str .= "\n";
    }

    return $res_str;

}

include_once( 'ut_common.inc' );

// Run the test
ut_run();
?>
--EXPECT--
Creating IntlDateFormatter with time_type = 0
After call to get_timetype :  timetype= 0

Creating IntlDateFormatter with time_type = 1
After call to get_timetype :  timetype= 1

Creating IntlDateFormatter with time_type = 2
After call to get_timetype :  timetype= 2

Creating IntlDateFormatter with time_type = 3
After call to get_timetype :  timetype= 3

Creating IntlDateFormatter with time_type = -1
After call to get_timetype :  timetype= -1
