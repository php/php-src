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

	function current()
	{
		self::fail(4, __FUNCTION__);
		return parent::current();
	}

	function key()
	{
		self::fail(5, __FUNCTION__);
		return parent::key();
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

	static function test($func)
	{
		echo "===$func===\n";
		self::$fail = 0;
		while(self::$fail < 10)
		{
			try
			{
				var_dump($func(new MyArrayIterator()));
				break;
			}
			catch (Exception $e)
			{
				var_dump($e->getMessage());
			}
			self::$fail++;
		}
	}
}

MyArrayIterator::test('iterator_to_array');
MyArrayIterator::test('iterator_count');

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
===iterator_to_array===
string(22) "State 0: __construct()"
string(22) "State 1: __construct()"
string(17) "State 2: rewind()"
string(16) "State 3: valid()"
string(18) "State 4: current()"
string(14) "State 5: key()"
string(15) "State 6: next()"
string(21) "State 7: __destruct()"
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
===iterator_count===
string(22) "State 0: __construct()"
string(22) "State 1: __construct()"
string(17) "State 2: rewind()"
string(16) "State 3: valid()"
string(15) "State 6: next()"
string(21) "State 7: __destruct()"
int(2)
===DONE===
