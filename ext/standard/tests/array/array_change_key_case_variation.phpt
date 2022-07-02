--TEST--
Test array_change_key_case() function - 2
--FILE--
<?php

$item = array ("one" => 1, "two" => 2, "THREE" => 3, "FOUR" => "four");

/* use 'case' argument other than CASE_LOWER & CASE_UPPER */
try {
    var_dump(array_change_key_case($item, "CASE_UPPER"));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump(array_change_key_case($item, 5));

/* when keys are different in terms of only case */
/* should return one value key pair with key being in lowercase */
var_dump( array_change_key_case( array("ONE" => 1, "one" => 3, "One" => 4) ) );

/* should return one value key pair with key being in uppercase */
var_dump( array_change_key_case( array("ONE" => 1, "one" => 2, "One" => 3), CASE_UPPER ) );
var_dump( array_change_key_case( array("ONE" => 1, "one" => 1, "One" => 2), 5 ) );

echo "end\n";
?>
--EXPECT--
array_change_key_case(): Argument #2 ($case) must be of type int, string given
array(4) {
  ["ONE"]=>
  int(1)
  ["TWO"]=>
  int(2)
  ["THREE"]=>
  int(3)
  ["FOUR"]=>
  string(4) "four"
}
array(1) {
  ["one"]=>
  int(4)
}
array(1) {
  ["ONE"]=>
  int(3)
}
array(1) {
  ["ONE"]=>
  int(2)
}
end
