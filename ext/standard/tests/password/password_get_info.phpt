--TEST--
Test normal operation of password_get_info()
--FILE--
<?php
//-=-=-=-
// Test Bcrypt
var_dump(password_get_info('$2y$10$MTIzNDU2Nzg5MDEyMzQ1Nej0NmcAWSLR.oP7XOR9HD/vjUuOj100y'));
// Test Bcrypt Cost
var_dump(password_get_info('$2y$11$MTIzNDU2Nzg5MDEyMzQ1Nej0NmcAWSLR.oP7XOR9HD/vjUuOj100y'));
// Test Bcrypt Invalid Length
var_dump(password_get_info('$2y$11$MTIzNDU2Nzg5MDEyMzQ1Nej0NmcAWSLR.oP7XOR9HD/vjUuOj100'));
// Test Non-Bcrypt
var_dump(password_get_info('$1$rasmusle$rISCgZzpwk3UhDidwXvin0'));

echo "OK!";
?>
--EXPECT--
array(3) {
  ["algo"]=>
  int(1)
  ["algoName"]=>
  string(6) "bcrypt"
  ["options"]=>
  array(1) {
    ["cost"]=>
    int(10)
  }
}
array(3) {
  ["algo"]=>
  int(1)
  ["algoName"]=>
  string(6) "bcrypt"
  ["options"]=>
  array(1) {
    ["cost"]=>
    int(11)
  }
}
array(3) {
  ["algo"]=>
  int(0)
  ["algoName"]=>
  string(7) "unknown"
  ["options"]=>
  array(0) {
  }
}
array(3) {
  ["algo"]=>
  int(0)
  ["algoName"]=>
  string(7) "unknown"
  ["options"]=>
  array(0) {
  }
}
OK!
