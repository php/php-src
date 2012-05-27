--TEST--
msgfmt_format() with named subpatterns
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
if (version_compare(INTL_ICU_VERSION, '4.8') < 0)
	die('skip for ICU 4.8+');
--FILE--
<?php

/*
 * Format a number using misc locales/patterns.
 */


function ut_main()
{

$pattern=<<<_MSG_
{gender_of_host, select,
  female {{num_guests, plural, offset:1
      =0 {{host} does not give a party.}
      =1 {{host} invites {guest} to her party.}
      =2 {{host} invites {guest} and one other person to her party.}
     other {{host} invites {guest} as one of the # people invited to her party.}}}
  male   {{num_guests, plural, offset:1
      =0 {{host} does not give a party.}
      =1 {{host} invites {guest} to his party.}
      =2 {{host} invites {guest} and one other person to his party.}
     other {{host} invites {guest} as one of the # people invited to his party.}}}
  other {{num_guests, plural, offset:1
      =0 {{host} does not give a party.}
      =1 {{host} invites {guest} to their party.}
      =2 {{host} invites {guest} and one other person to their party.}
     other {{host} invites {guest} as one of the # people invited to their party.}}}}
_MSG_;


$args = array(
      array('gender_of_host' => 'female', 'num_guests' => 0, 'host' => 'Alice', 'guest' => 'Bob'),
      array('gender_of_host' => 'male', 'num_guests' => 1, 'host' => 'Alice', 'guest' => 'Bob'),
      array('gender_of_host' => 'none', 'num_guests' => 2, 'host' => 'Alice', 'guest' => 'Bob'),
      array('gender_of_host' => 'female', 'num_guests' => 27, 'host' => 'Alice', 'guest' => 'Bob'),
);

$str_res = '';

        $fmt = ut_msgfmt_create( 'en_US', $pattern );
		if(!$fmt) {
			$str_res .= dump(intl_get_error_message())."\n";
			return $str_res;
		}
        foreach ($args as $arg) {
            $str_res .= dump( ut_msgfmt_format($fmt, $arg) ). "\n";
            $str_res .= dump( ut_msgfmt_format_message('en_US', $pattern, $arg) ) . "\n";
    }
    return $str_res;
}

include_once( 'ut_common.inc' );

// Run the test
ut_run();

?>
--EXPECT--
'Alice does not give a party.'
'Alice does not give a party.'
'Alice invites Bob to his party.'
'Alice invites Bob to his party.'
'Alice invites Bob and one other person to their party.'
'Alice invites Bob and one other person to their party.'
'Alice invites Bob as one of the 26 people invited to her party.'
'Alice invites Bob as one of the 26 people invited to her party.'
