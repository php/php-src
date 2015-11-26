--TEST--
Bug #30407 (Strange behaviour of default arguments)
--FILE--
<?php

function haricow($a = 'one') {
	var_dump($a);
	$a = 'two';
}

haricow();
haricow();
?>
===DONE===
--EXPECT--
string(3) "one"
string(3) "one"
===DONE===
