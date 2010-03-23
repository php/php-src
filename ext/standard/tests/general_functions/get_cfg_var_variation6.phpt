--TEST--
Test function get_cfg_var() by substituting argument 1 with object values.
--CREDITS--
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--INI--
session.use_cookies=0
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php


echo "*** Test substituting argument 1 with object values ***\n";



class classWithToString
{
        public function __toString() {
                return "session.use_cookies";
        }
}

class classWithoutToString
{
}

$variation_array = array(
  'instance of classWithToString' => new classWithToString(),
  'instance of classWithoutToString' => new classWithoutToString(),
  );


foreach ( $variation_array as $var ) {
  var_dump(get_cfg_var( $var  ) );
}
?>
--EXPECTF--
*** Test substituting argument 1 with object values ***
string(1) "0"

Warning: get_cfg_var() expects parameter 1 to be string, object given in %s.php on line %d
NULL
