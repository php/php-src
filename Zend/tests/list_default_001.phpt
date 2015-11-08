--TEST--
Testing list() default value: basic
--FILE--
<?php

// empty
list($a=1) = array();
var_dump($a);

list($a=1, $b=2) = array();
var_dump($a, $b);

list($a, $b=2) = array();
var_dump($a, $b);

echo "---------\n";

list($a=1) = array(2);
var_dump($a);

list($a=1, $b=2) = array(1 => 3);
var_dump($a, $b);

list($a, $b=2, $c) = array();
var_dump($a, $b, $c);

?>
--EXPECTF--
int(1)
int(1)
int(2)

Notice: Undefined offset: 0 in %s on line %s
NULL
int(2)
---------
int(2)
int(1)
int(3)

Notice: Undefined offset: %d in %s on line %s

Notice: Undefined offset: %d in %s on line %s
NULL
int(2)
NULL
