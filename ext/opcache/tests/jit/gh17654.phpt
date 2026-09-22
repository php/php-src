--TEST--
GH-17654 (Multiple classes using same trait causes function JIT crash)
--EXTENSIONS--
opcache
--INI--
opcache.jit=1214
opcache.jit_buffer_size=16M
--FILE--
<?php
trait TestTrait {
	public function addUnit(string $x) {
		self::addRawUnit($this, $x);
	}

	public function addRawUnit(self $data, string $x) {
		var_dump($x);
	}
}

class Test {
	use TestTrait;
}

class Test2 {
	use TestTrait;
}

function main()
{
	(new Test2)->addUnit("test2");
	(new Test)->addUnit("test");
}

main();
?>
--EXPECT--
string(5) "test2"
string(4) "test"
