--TEST--
datefmt_get_locale_code()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

/*
 * Test for the datefmt_get_locale  function
 */


function ut_main()
{
	$locale_arr = array (
		'de-DE',
		'sl-IT-nedis',
		'en_UK',
		'hi'
	);
	
	$res_str = '';

	foreach( $locale_arr as $locale_entry )
	{
		$res_str .= "\nCreating IntlDateFormatter with locale = $locale_entry";
		$fmt = ut_datefmt_create( $locale_entry , IntlDateFormatter::SHORT,IntlDateFormatter::SHORT,'America/Los_Angeles', IntlDateFormatter::GREGORIAN  );
		$locale = ut_datefmt_get_locale( $fmt , 1);
		$res_str .= "\nAfter call to get_locale :  locale= $locale";
		$res_str .= "\n";
	}
	$badvals = array(100, -1, 4294901761);
	foreach($badvals as $badval) {
		if(ut_datefmt_get_locale($fmt, $badval)) {
			$res_str .= "datefmt_get_locale should return false for bad argument $badval\n";
		}
	}

	return $res_str;

}

include_once( 'ut_common.inc' );

// Run the test
ut_run();
?>
--EXPECT--
Creating IntlDateFormatter with locale = de-DE
After call to get_locale :  locale= de_DE

Creating IntlDateFormatter with locale = sl-IT-nedis
After call to get_locale :  locale= sl

Creating IntlDateFormatter with locale = en_UK
After call to get_locale :  locale= en

Creating IntlDateFormatter with locale = hi
After call to get_locale :  locale= hi
