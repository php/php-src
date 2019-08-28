--TEST--
Bug #67922: Member function not found when dereferencing in write-context
--FILE--
<?php

class C {
	public function test() {
		return new StdClass;
	}
}

$b = new StdClass;
$b->c = new C;
$b->c->test()->d = 'str';

?>
===DONE===
--EXPECT--
===DONE===
