--TEST--
Test function fstat() by substituting agument 1 with string values.
--FILE--
<?php
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
  var_dump(fstat( $var  ) );
}
?>
===DONE===
--EXPECTF--

Warning: fstat() expects parameter 1 to be resource, string given in %s on line %d
bool(false)

Warning: fstat() expects parameter 1 to be resource, string given in %s on line %d
bool(false)

Warning: fstat() expects parameter 1 to be resource, string given in %s on line %d
bool(false)

Warning: fstat() expects parameter 1 to be resource, string given in %s on line %d
bool(false)
===DONE===