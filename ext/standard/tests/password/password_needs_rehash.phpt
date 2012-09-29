--TEST--
Test normal operation of password_needs_rehash()
--FILE--
<?php
//-=-=-=-

// Invalid Hash, always rehash
var_dump(password_needs_rehash('', PASSWORD_BCRYPT));

// Valid, as it's an unknown algorithm
var_dump(password_needs_rehash('', 0));

// Valid with cost the same
var_dump(password_needs_rehash('$2y$10$MTIzNDU2Nzg5MDEyMzQ1Nej0NmcAWSLR.oP7XOR9HD/vjUuOj100y', PASSWORD_BCRYPT, array('cost' => 10)));

// Valid with cost the same, additional params
var_dump(password_needs_rehash('$2y$10$MTIzNDU2Nzg5MDEyMzQ1Nej0NmcAWSLR.oP7XOR9HD/vjUuOj100y', PASSWORD_BCRYPT, array('cost' => 10, 'foo' => 3)));

// Invalid, different (lower) cost
var_dump(password_needs_rehash('$2y$10$MTIzNDU2Nzg5MDEyMzQ1Nej0NmcAWSLR.oP7XOR9HD/vjUuOj100y', PASSWORD_BCRYPT, array('cost' => 09)));

// Invalid, different (higher) cost
var_dump(password_needs_rehash('$2y$10$MTIzNDU2Nzg5MDEyMzQ1Nej0NmcAWSLR.oP7XOR9HD/vjUuOj100y', PASSWORD_BCRYPT, array('cost' => 11)));

// Valid with cost the default
$cost = str_pad(PASSWORD_BCRYPT_DEFAULT_COST, 2, '0', STR_PAD_LEFT);
var_dump(password_needs_rehash('$2y$'.$cost.'$MTIzNDU2Nzg5MDEyMzQ1Nej0NmcAWSLR.oP7XOR9HD/vjUuOj100y', PASSWORD_BCRYPT));

// Should Issue Needs Rehash, Since Foo is cast to 0...
var_dump(password_needs_rehash('$2y$10$MTIzNDU2Nzg5MDEyMzQ1Nej0NmcAWSLR.oP7XOR9HD/vjUuOj100y', PASSWORD_BCRYPT, array('cost' => 'foo')));



echo "OK!";
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
OK!
