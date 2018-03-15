--TEST--
Test function get_cfg_var() by substituting argument 1 with string values.
--CREDITS--
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--INI--
session.use_cookies=0
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php


echo "*** Test substituting argument 1 with unknown string values ***\n";



$heredoc = <<<EOT
hello world
EOT;

$variation_array = array(
  'string DQ' => "string",
  'string SQ' => 'string',
  'mixed case string' => "sTrInG",
  'heredoc' => $heredoc
  );


foreach ( $variation_array as $var ) {
  var_dump(get_cfg_var( $var  ) );
}
?>
--EXPECT--
*** Test substituting argument 1 with unknown string values ***
bool(false)
bool(false)
bool(false)
bool(false)
