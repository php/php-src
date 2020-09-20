--TEST--
msgfmt_format() with subpatterns
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php

/*
 * Format a number using misc locales/patterns.
 */


function ut_main()
{

$pattern=<<<_MSG_
{0, select,
  female {{1, plural, offset:1
      =0 {{2} does not give a party.}
      =1 {{2} invites {3} to her party.}
      =2 {{2} invites {3} and one other person to her party.}
     other {{2} invites {3} as one of the # people invited to her party.}}}
  male   {{1, plural, offset:1
      =0 {{2} does not give a party.}
      =1 {{2} invites {3} to his party.}
      =2 {{2} invites {3} and one other person to his party.}
     other {{2} invites {3} as one of the # other people invited to his party.}}}
  other {{1, plural, offset:1
      =0 {{2} does not give a party.}
      =1 {{2} invites {3} to their party.}
      =2 {{2} invites {3} and one other person to their party.}
      other {{2} invites {3} as one of the # other people invited to their party.}}}}
_MSG_;


$args = array(
      array('female', 0,  'Alice', 'Bob'),
      array('male',   1,  'Alice', 'Bob'),
      array('none',   2,  'Alice', 'Bob'),
      array('female', 27, 'Alice', 'Bob'),
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
