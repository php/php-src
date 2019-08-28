--TEST--
Testing type-hinted lambda parameter inside namespace
--FILE--
<?php

namespace foo;

$x = function (\StdClass $x = NULL) {
	var_dump($x);
};

class StdClass extends \StdClass { }

$x(NULL);
$x(new StdClass);
$x(new \StdClass);

?>
--EXPECTF--
NULL
object(foo\StdClass)#%d (0) {
}
object(StdClass)#%d (0) {
}
