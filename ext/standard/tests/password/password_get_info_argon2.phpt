--TEST--
Test normal operation of password_get_info() with Argon2i and Argon2id
--SKIPIF--
<?php
if (!defined('PASSWORD_ARGON2I')) die('skip password_get_info not built with Argon2');
if (!defined('PASSWORD_ARGON2ID')) die('skip password_get_info not built with Argon2');
?>
--FILE--
<?php

var_dump(password_get_info('$argon2i$v=19$m=65536,t=3,p=1$SWhIcG5MT21Pc01PbWdVZw$WagZELICsz7jlqOR2YzoEVTWb2oOX1tYdnhZYXxptbU'));
var_dump(password_get_info('$argon2id$v=19$m=1024,t=2,p=2$Zng1U1RHS0h1aUJjbGhPdA$ajQnG5s01Ws1ad8xv+1qGfXF8mYxxWdyul5rBpomuZQ'));
echo "OK!";
?>
--EXPECT--
array(3) {
  ["algo"]=>
  int(2)
  ["algoName"]=>
  string(7) "argon2i"
  ["options"]=>
  array(3) {
    ["memory_cost"]=>
    int(65536)
    ["time_cost"]=>
    int(3)
    ["threads"]=>
    int(1)
  }
}
array(3) {
  ["algo"]=>
  int(3)
  ["algoName"]=>
  string(8) "argon2id"
  ["options"]=>
  array(3) {
    ["memory_cost"]=>
    int(1024)
    ["time_cost"]=>
    int(2)
    ["threads"]=>
    int(2)
  }
}
OK!
