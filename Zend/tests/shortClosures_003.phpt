--TEST--
Short Closures implicit use must be throwing notices only upon actual use
--FILE--
<?php

$b = "\nok";

(() ~> {
	$a = "foo"; echo $a;
	echo $b;
	echo $c;
})();

?>
--EXPECTF--
foo
ok
Notice: Undefined variable: c in %s on line %d
