--TEST--
Bug #71092 (Segmentation fault with return type hinting)
--INI--
opcache.enable=0
--FILE--
<?php

function boom(): array {
	$data = [['id']];
	switch ($data[0]) {
	case ['id']:
		return;
	}
}

boom();
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Return value of boom() must be of the type array, none returned in %sbug71092.php:%d
Stack trace:
#0 %s(%d): boom()
#1 {main}
  thrown in %sbug71092.php on line %d
