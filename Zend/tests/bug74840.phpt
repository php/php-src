--TEST--
Bug #74840: Opcache overwrites argument of GENERATOR_RETURN within finally
--FILE--
<?php

$g = (function($a) {
	try {
		return $a + 1;
	} finally {
		$b = $a + 2;
		var_dump($b);
	}
	yield; // Generator
})(1);
$g->next();
var_dump($g->getReturn());

?>
--EXPECT--
int(3)
int(2)
