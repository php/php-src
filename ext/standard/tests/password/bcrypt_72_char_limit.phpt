--TEST--
Test 72 character limit of bcrypt
--FILE--
<?php
$long_pass = str_repeat('a', 71);

$hash = password_hash($long_pass . 'a', PASSWORD_BCRYPT, array("cost" => 4));
var_dump(password_verify($long_pass . 'a', $hash));
var_dump(password_verify($long_pass . 'b', $hash));

$hash = password_hash($long_pass . 'aa', PASSWORD_BCRYPT, array("cost" => 4));
var_dump(password_verify($long_pass . 'aa', $hash));

echo "This is the unexpected behavior we warn about: password is different but password_verify returns true.\n";
var_dump(password_verify($long_pass . 'ab', $hash));

?>
--EXPECTF--
bool(true)
bool(false)

Notice: Passwords longer than 72 characters are truncated by bcrypt in %s on line %d
bool(true)
This is the unexpected behavior we warn about: password is different but password_verify returns true.
bool(true)
