--TEST--
Test password_get_info() with bcrypt-sha256
--FILE--
<?php
//-=-=-=-

// Default cost (12)
var_dump(password_get_info(password_hash("foo", PASSWORD_BCRYPT_SHA256)));

// 1-digit cost
var_dump(password_get_info(password_hash("foo", PASSWORD_BCRYPT_SHA256, ["cost" => 4])));

// Malformed hash is reported as unknown
var_dump(password_get_info('$bcrypt-sha256$v=2,t=2b,r=12$bad-salt!!$Kq4Noyk3094Y2QlB8NdRT8SvGiI4ft2'));

echo "OK!";
?>
--EXPECT--
array(3) {
  ["algo"]=>
  string(13) "bcrypt-sha256"
  ["algoName"]=>
  string(13) "bcrypt-sha256"
  ["options"]=>
  array(1) {
    ["cost"]=>
    int(12)
  }
}
array(3) {
  ["algo"]=>
  string(13) "bcrypt-sha256"
  ["algoName"]=>
  string(13) "bcrypt-sha256"
  ["options"]=>
  array(1) {
    ["cost"]=>
    int(4)
  }
}
array(3) {
  ["algo"]=>
  NULL
  ["algoName"]=>
  string(7) "unknown"
  ["options"]=>
  array(0) {
  }
}
OK!
