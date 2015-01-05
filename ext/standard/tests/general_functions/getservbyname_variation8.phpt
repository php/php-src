--TEST--
Test function getservbyname() by substituting argument 2 with array values.
--FILE--
<?php


echo "*** Test substituting argument 2 with array values ***\n";

$service = "www";


$index_array = array(1, 2, 3);
$assoc_array = array(1 => 'one', 2 => 'two');

$variation_array = array(
  'empty array' => array(),
  'int indexed array' => $index_array,
  'associative array' => $assoc_array,
  'nested arrays' => array('foo', $index_array, $assoc_array),
  );


foreach ( $variation_array as $var ) {
  var_dump(getservbyname( $service, $var  ) );
}
?>
--EXPECTF--
*** Test substituting argument 2 with array values ***

Warning: getservbyname() expects parameter 2 to be string, array given in %s on line %d
NULL

Warning: getservbyname() expects parameter 2 to be string, array given in %s on line %d
NULL

Warning: getservbyname() expects parameter 2 to be string, array given in %s on line %d
NULL

Warning: getservbyname() expects parameter 2 to be string, array given in %s on line %d
NULL
