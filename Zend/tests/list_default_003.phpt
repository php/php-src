--TEST--
Testing list() default value: non array
--FILE--
<?php

list($a, $b=2) = 1;
var_dump($a, $b);

list($a, $b=2) = null;
var_dump($a, $b);

list($a, $b=2) = "rule the world!";
var_dump($a, $b);


list($a=10, list($b, $c=20), $d) = null;
var_dump($a, $b, $c, $d);

?>
--EXPECT--
NULL
int(2)
NULL
int(2)
NULL
int(2)
int(10)
NULL
int(20)
NULL
