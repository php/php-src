--TEST--
Test function getservbyname() by substituting argument 1 with string values.
--FILE--
<?php


echo "*** Test substituting argument 1 with string values ***\n";

$protocol = "tcp";


$heredoc = <<<EOT
hello world
EOT;

$variation_array = array(
  'string DQ' => "string",
  'string SQ' => 'string',
  'mixed case string' => "sTrInG",
  'heredoc' => $heredoc,
  );


foreach ( $variation_array as $var ) {
  var_dump(getservbyname( $var ,  $protocol ) );
}
?>
--EXPECTF--
*** Test substituting argument 1 with string values ***
bool(false)
bool(false)
bool(false)
bool(false)
