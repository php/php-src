--TEST--
Naming a function readonly is deprecated
--FILE--
<?php

class C {
	public function readonly() {}
}

?>
DONE
--EXPECT--
DONE
