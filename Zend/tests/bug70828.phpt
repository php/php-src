--TEST--
Bug #70828 (php-fpm 5.6 with opcache crashes when referencing a non-existent constant)
--FILE--
<?php

namespace test {
	use const nonexistent;

	class test {
		static function run(){
			var_dump(nonexistent);
			existent;
		}
	}
}

namespace {
	define("test\\existent", "bug!", 1);
	test\test::run();
}
?>
--EXPECTF--
Notice: Use of undefined constant nonexistent - assumed 'nonexistent' in %sbug70828.php on line %d
string(11) "nonexistent"
