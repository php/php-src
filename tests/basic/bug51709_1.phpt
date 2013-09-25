--TEST--
Bug #51709 (Use keywords as method or class names)
--FILE--
<?php

class for {
	static function foreach() {
		echo "OK";
	}
}

for::foreach();

?>
--EXPECTF--
OK
