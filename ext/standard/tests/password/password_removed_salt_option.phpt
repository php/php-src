--TEST--
Test removed support for explicit salt option
--SKIPIF--
<?php if (getenv("SKIP_SLOW_TESTS")) die("skip slow test"); ?>
--FILE--
<?php
//-=-=-=-


var_dump(strlen(password_hash("rasmuslerdorf", PASSWORD_BCRYPT, array("cost" => 7, "salt" => "usesomesillystringforsalt"))));

var_dump(strlen(password_hash("test", PASSWORD_BCRYPT, array("salt" => "123456789012345678901" . chr(0)))));

echo "OK!";
?>
--EXPECTF--
Warning: password_hash(): The "salt" option has been ignored, since providing a custom salt is no longer supported in %s on line %d
int(60)

Warning: password_hash(): The "salt" option has been ignored, since providing a custom salt is no longer supported in %s on line %d
int(60)
OK!
