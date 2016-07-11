--TEST--
Test normal operation of password_get_info() with Argon2
--SKIPIF--
<?php
if (!defined('PASSWORD_ARGON2')) die('Skipped: password_get_info not built with Argon2');
?>
--FILE--
<?php
// Test Argon2i
var_dump(password_get_info('$argon2i$v=19$m=65536,t=3,p=1$SWhIcG5MT21Pc01PbWdVZw$WagZELICsz7jlqOR2YzoEVTWb2oOX1tYdnhZYXxptbU'));
// Test Argon2d
var_dump(password_get_info('$argon2d$v=19$m=32768,t=2,p=1$YWpxd0VYRW9MLmp6VjFPZw$pWV5IsbBfjEK5c0bHzvAo0FsDNHUyM4p6j8vf2cxzb8'));
echo "OK!";
?>
--EXPECT--
array(3) {
  ["algo"]=>
  int(3)
  ["algoName"]=>
  string(7) "argon2i"
  ["options"]=>
  array(3) {
    ["m_cost"]=>
    int(65536)
    ["t_cost"]=>
    int(3)
    ["threads"]=>
    int(1)
  }
}
array(3) {
  ["algo"]=>
  int(2)
  ["algoName"]=>
  string(7) "argon2d"
  ["options"]=>
  array(3) {
    ["m_cost"]=>
    int(32768)
    ["t_cost"]=>
    int(2)
    ["threads"]=>
    int(1)
  }
}
OK!