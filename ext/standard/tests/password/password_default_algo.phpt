--TEST--
Test password_default_algo()
--FILE--
<?php
// Test Bcrypt
var_dump(password_default_algo());
--EXPECT--
array(3) {
  ["algo"]=>
  string(2) "2y"
  ["algoName"]=>
  string(6) "bcrypt"
  ["options"]=>
  array(1) {
    ["cost"]=>
    int(10)
  }
}
