--TEST--
Test deprecated operation of password_hash()
--FILE--
<?php
//-=-=-=-


var_dump(password_hash("rasmuslerdorf", PASSWORD_BCRYPT, array("cost" => 7, "salt" => "usesomesillystringforsalt")));

var_dump(password_hash("test", PASSWORD_BCRYPT, array("salt" => "123456789012345678901" . chr(0))));

echo "OK!";
?>
--EXPECTF--
Deprecated: password_hash(): Use of the 'salt' option to password_hash is deprecated in %s on line %d
string(60) "$2y$07$usesomesillystringfore2uDLvp1Ii2e./U9C8sBjqp8I90dH6hi"

Deprecated: password_hash(): Use of the 'salt' option to password_hash is deprecated in %s on line %d
string(60) "$2y$10$MTIzNDU2Nzg5MDEyMzQ1Nej0NmcAWSLR.oP7XOR9HD/vjUuOj100y"
OK!

