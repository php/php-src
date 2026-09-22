--TEST--
casting to void destroys the value.
--FILE--
<?php

class WithDestructor {
	public function __destruct() {
		echo __METHOD__, PHP_EOL;
	}
}

function test(): WithDestructor {
	return new WithDestructor();
}

function do_it(): void {
	echo "Before", PHP_EOL;

	(void)test();

	echo "After", PHP_EOL;
}

do_it();

?>
--EXPECT--
Before
WithDestructor::__destruct
After
