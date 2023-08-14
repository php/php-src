--TEST--
Disable non-internal class
--INI--
disable_classes=UserLand
--FILE--
<?php

class UserLand {}
$o = new UserLand();
var_dump($o);
?>
DONE
--EXPECT--
object(UserLand)#1 (0) {
}
DONE
