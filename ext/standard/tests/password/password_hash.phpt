--TEST--
Test normal operation of password_hash()
--FILE--
<?php
//-=-=-=-

// Set the cost low so the test is fast
ini_set('password.bcrypt_cost', '4');

var_dump(strlen(password_hash("foo")));

$hash = password_hash("foo");

var_dump($hash == crypt("foo", $hash));

var_dump(password_hash("rasmuslerdorf", PASSWORD_BCRYPT, array("cost" => 7, "salt" => "usesomesillystringforsalt")));

var_dump(password_hash("test", PASSWORD_BCRYPT, array("salt" => "123456789012345678901" . chr(0))));

echo "OK!";
?>
--EXPECT--
int(60)
bool(true)
string(60) "$2y$07$usesomesillystringfore2uDLvp1Ii2e./U9C8sBjqp8I90dH6hi"
string(60) "$2y$04$MTIzNDU2Nzg5MDEyMzQ1NekACxf2CF7ipfk/b9FllU9Fs8RcUm5UG"
OK!
