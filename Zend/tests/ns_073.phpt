--TEST--
Testing type-hinted lambda parameter inside namespace
--FILE--
<?php

namespace foo;

$x = function (\StdClass $x = NULL) {
	var_dump($x);
};

$x(NULL);
$x(new \StdClass);

?>
--EXPECTF--
NULL
object(StdClass)#%d (0) {
}
