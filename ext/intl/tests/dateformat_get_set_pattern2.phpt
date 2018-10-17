--TEST--
datefmt_get_pattern_code and datefmt_set_pattern_code() icu >= 4.8
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip intl extension not loaded'; ?>
--FILE--
<?php

/*
 * Test for the datefmt_get_pattern & datefmt_set_pattern function
 */


function ut_main()
{
        $pattern_arr = array (
                'DD-MM-YYYY hh:mm:ss',
		'yyyy-DDD.hh:mm:ss z',
                "yyyy/MM/dd",
                "yyyyMMdd"
        );

        $res_str = '';

        $start_pattern = 'dd-MM-YY';
        $res_str .= "\nCreating IntlDateFormatter with pattern = $start_pattern ";
        //$fmt = ut_datefmt_create( "en-US",  IntlDateFormatter::SHORT, IntlDateFormatter::SHORT , 'America/New_York', IntlDateFormatter::GREGORIAN , $start_pattern );
        $fmt = ut_datefmt_create( "en-US",  IntlDateFormatter::FULL, IntlDateFormatter::FULL, 'America/New_York', IntlDateFormatter::GREGORIAN , $start_pattern );
        $pattern = ut_datefmt_get_pattern( $fmt);
        $res_str .= "\nAfter call to get_pattern :  pattern= $pattern";
	$formatted = ut_datefmt_format($fmt,0);
	$res_str .= "\nResult of formatting timestamp=0 is :  \n$formatted";


        foreach( $pattern_arr as $pattern_entry )
        {
                $res_str .= "\n-------------------";
                $res_str .= "\nSetting IntlDateFormatter with pattern = $pattern_entry ";
                ut_datefmt_set_pattern( $fmt , $pattern_entry );
                $pattern = ut_datefmt_get_pattern( $fmt);
                $res_str .= "\nAfter call to get_pattern :  pattern= $pattern";
		$formatted = ut_datefmt_format($fmt,0);
                $res_str .= "\nResult of formatting timestamp=0 with the new pattern is :  \n$formatted";
                $res_str .= "\n";

        }

        return $res_str;

}

include_once( 'ut_common.inc' );

// Run the test
ut_run();
?>
--EXPECT--
Creating IntlDateFormatter with pattern = dd-MM-YY 
After call to get_pattern :  pattern= dd-MM-YY
Result of formatting timestamp=0 is :  
31-12-70
-------------------
Setting IntlDateFormatter with pattern = DD-MM-YYYY hh:mm:ss 
After call to get_pattern :  pattern= DD-MM-YYYY hh:mm:ss
Result of formatting timestamp=0 with the new pattern is :  
365-12-1970 07:00:00

-------------------
Setting IntlDateFormatter with pattern = yyyy-DDD.hh:mm:ss z 
After call to get_pattern :  pattern= yyyy-DDD.hh:mm:ss z
Result of formatting timestamp=0 with the new pattern is :  
1969-365.07:00:00 EST

-------------------
Setting IntlDateFormatter with pattern = yyyy/MM/dd 
After call to get_pattern :  pattern= yyyy/MM/dd
Result of formatting timestamp=0 with the new pattern is :  
1969/12/31

-------------------
Setting IntlDateFormatter with pattern = yyyyMMdd 
After call to get_pattern :  pattern= yyyyMMdd
Result of formatting timestamp=0 with the new pattern is :  
19691231
