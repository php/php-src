--TEST--
SPL: iterator_to_array() and exceptions
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

class MyArrayIterator extends ArrayIterator
{
	static protected $fail = 0;

	static function fail($state, $method)
	{
		if (self::$fail == $state)
		{
			throw new Exception("State $state: $method()");
		}
	}

	function __construct()
	{
		self::fail(0, __FUNCTION__);
		parent::__construct(array(1, 2));
		self::fail(1, __FUNCTION__);
	}

	function rewind()
	{
		self::fail(2, __FUNCTION__);
		return parent::rewind();
	}

	function valid()
	{
		self::fail(3, __FUNCTION__);
		return parent::valid();
	}

	function key()
	{
		self::fail(4, __FUNCTION__);
		return parent::key();
	}

	function current()
	{
		self::fail(5, __FUNCTION__);
		return parent::current();
	}

	function next()
	{
		self::fail(6, __FUNCTION__);
		return parent::next();
	}

	function __destruct()
	{
		self::fail(7, __FUNCTION__);
	}

	static function test()
	{
		self::$fail = 0;
		while(self::$fail < 10)
		{
			echo '===' . self::$fail . "===\n";
			try
			{
				var_dump(iterator_to_array(new MyArrayIterator()));
				break;
			}
			catch (Exception $e)
			{
				var_dump($e->getMessage());
			}
			self::$fail++;
		}

		self::$fail = 0;
		while(self::$fail < 10)
		{
			echo '===' . self::$fail . "===\n";
			try
			{
				var_dump(iterator_count(new MyArrayIterator()));
				break;
			}
			catch (Exception $e)
			{
				var_dump($e->getMessage());
			}
			if (self::$fail == 3)
			{
				self::$fail = 6;
			}
			else
			{
				self::$fail++;
			}
		}
	}
}

MyArrayIterator::test();

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
===0===
string(22) "State 0: __construct()"
===1===
string(22) "State 1: __construct()"
===2===
string(17) "State 2: rewind()"
===3===
string(16) "State 3: valid()"
===4===
string(14) "State 4: key()"
===5===
string(18) "State 5: current()"
===6===
string(15) "State 6: next()"
===7===
string(21) "State 7: __destruct()"
===8===
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
===0===
string(22) "State 0: __construct()"
===1===
string(22) "State 1: __construct()"
===2===
string(17) "State 2: rewind()"
===3===
string(16) "State 3: valid()"
===4===
string(15) "State 6: next()"
===7===
string(21) "State 7: __destruct()"
===8===
int(2)
===DONE===
