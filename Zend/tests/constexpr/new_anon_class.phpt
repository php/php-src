--TEST--
New with anonymous class works
--FILE--
<?php

static $x = new class {};

var_dump($x);

?>
--EXPECTF--
object(class@anonymous%0%s:3$%x)#1 (0) {
}
