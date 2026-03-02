--TEST--
New with anonymous class works
--FILE--
<?php

static $x = new class {};

var_dump($x);

?>
--EXPECT--
object(class@anonymous)#1 (0) {
}
