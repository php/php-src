--TEST--
Temporary leak on rope (encapsed string)
--FILE--
<?php
class Obj {
	function __get($x) {
		throw new Exception();
	}
}

$x = new Obj;
$y = 0;

try {
	$r = "$y|$x->x|";
} catch (Exception $e) {
}

try {
	$r = "$x->x|$y|";
} catch (Exception $e) {
}

try {
	$r = "$y|$y|$x->x";
} catch (Exception $e) {
}

?>
==DONE==
--EXPECT--
==DONE==
