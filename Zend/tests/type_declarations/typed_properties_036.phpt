--TEST--
Test typed properties normal foreach must not yield null
--FILE--
<?php
$foo = new class {
	public int $bar = 10,
			int $qux;
};

foreach ($foo as $key => $bar)
	var_dump($key, $bar);
--EXPECTF--
string(3) "bar"
int(10)

Fatal error: Uncaught TypeError: Typed property class@anonymous::$qux must not be accessed before initialization in %s:7
Stack trace:
#0 {main}
  thrown in %s on line 7

