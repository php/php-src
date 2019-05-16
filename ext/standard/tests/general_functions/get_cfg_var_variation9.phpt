--TEST--
Test function get_cfg_var() by substituting argument with array of valid parameters.
--CREDITS--
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--INI--
session.use_cookies=0
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php


echo "*** Test substituting argument with array of valid parameters ***\n";



$heredoc = <<<EOT
hello world
EOT;

$variation_array = array(
  'session.use_cookies',
  'session.serialize_handler',
  'session.save_handler'
  );


foreach ( $variation_array as $var ) {
  var_dump(get_cfg_var( $var  ) );
}
?>
--EXPECT--
*** Test substituting argument with array of valid parameters ***
string(1) "0"
string(3) "php"
string(5) "files"
