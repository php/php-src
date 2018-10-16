--TEST--
Test function getservbyname() by substituting argument 2 with string values.
--FILE--
<?php


echo "*** Test substituting argument 2 with string values ***\n";

$service = "www";


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
  var_dump(getservbyname( $service, $var  ) );
}
?>
--EXPECT--
*** Test substituting argument 2 with string values ***
bool(false)
bool(false)
bool(false)
bool(false)
