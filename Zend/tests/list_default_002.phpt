--TEST--
Testing list() default value: nested list
--FILE--
<?php

list($a, list($b=1, $c), $d=99) = [1, [1 => new stdclass]];
var_dump($a, $b, $c, $d);

?>
--EXPECT--
int(1)
int(1)
object(stdClass)#1 (0) {
}
int(99)
