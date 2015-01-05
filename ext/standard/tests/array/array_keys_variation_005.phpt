--TEST--
Test array_keys() function (variation - 5)
--FILE--
<?php

echo "\n*** Testing array_keys() with resource type ***\n";
$resource1 = fopen( __FILE__, "r");
$resource2 = opendir( "." );

/* creating an array with resource types as elements */
$arr_resource = array($resource1, $resource2); 

var_dump(array_keys($arr_resource, $resource1));  // loose type checking 
var_dump(array_keys($arr_resource, $resource1, TRUE));  // strict type checking
var_dump(array_keys($arr_resource, $resource2));  // loose type checking
var_dump(array_keys($arr_resource, $resource2, TRUE));  // strict type checking

/* Closing the resource handles */
fclose( $resource1 );
closedir( $resource2 );

?>
--EXPECTF--
*** Testing array_keys() with resource type ***
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(1)
}
