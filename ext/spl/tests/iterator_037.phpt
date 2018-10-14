--TEST--
SPL: CachingIterator and __toString
--FILE--
<?php

function test($ar, $flags)
{
	echo "===$flags===\n";
	$it = new CachingIterator($ar, 0);
	try
	{
		$it->setFlags($flags);
	}
	catch (Exception $e)
	{
		echo 'Exception: ' . $e->getMessage() . "\n";
		var_dump($it->getFlags());
		return;
	}
	var_dump($it->getFlags());
	try
	{
		foreach($it as $v)
		{
			var_dump((string)$it);
		}
	}
	catch (Exception $e)
	{
		echo 'Exception: ' . $e->getMessage() . "\n";
	}
}

class MyItem
{
	function __construct($value)
	{
		$this->value = $value;
	}

	function __toString()
	{
		return (string)$this->value;
	}
}

class MyArrayIterator extends ArrayIterator
{
	function __toString()
	{
		return $this->key() . ':' . $this->current();
	}
}

$ar = new MyArrayIterator(array(1, 2, 3));

test($ar, CachingIterator::CALL_TOSTRING);
test($ar, CachingIterator::TOSTRING_USE_KEY);
test($ar, CachingIterator::TOSTRING_USE_CURRENT);

$ar = new MyArrayIterator(array(new MyItem(1), new MyItem(2), new MyItem(3)));

test($ar, CachingIterator::TOSTRING_USE_INNER);
test($ar, CachingIterator::CALL_TOSTRING | CachingIterator::TOSTRING_USE_KEY);
test($ar, CachingIterator::CALL_TOSTRING | CachingIterator::TOSTRING_USE_CURRENT);
test($ar, CachingIterator::CALL_TOSTRING | CachingIterator::TOSTRING_USE_INNER);
test($ar, CachingIterator::TOSTRING_USE_KEY | CachingIterator::TOSTRING_USE_CURRENT);
test($ar, CachingIterator::TOSTRING_USE_KEY | CachingIterator::TOSTRING_USE_INNER);

echo "===X===\n";
try
{
	$it = new CachingIterator($ar, CachingIterator::CALL_TOSTRING);
	$it->setFlags(0);
}
catch (Exception $e)
{
	echo 'Exception: ' . $e->getMessage() . "\n";
}
try
{
	$it = new CachingIterator($ar, CachingIterator::TOSTRING_USE_INNER);
	$it->setFlags(0);
}
catch (Exception $e)
{
	echo 'Exception: ' . $e->getMessage() . "\n";
}

?>
===DONE===
--EXPECTF--
===1===
int(1)
string(1) "1"
string(1) "2"
string(1) "3"
===2===
int(2)
string(1) "0"
string(1) "1"
string(1) "2"
===4===
int(4)
string(1) "1"
string(1) "2"
string(1) "3"
===8===
int(8)
string(3) "0:1"
string(3) "1:2"
string(3) "2:3"
===3===
Exception: Flags must contain only one of CALL_TOSTRING, TOSTRING_USE_KEY, TOSTRING_USE_CURRENT, TOSTRING_USE_INNER
int(0)
===5===
Exception: Flags must contain only one of CALL_TOSTRING, TOSTRING_USE_KEY, TOSTRING_USE_CURRENT, TOSTRING_USE_INNER
int(0)
===9===
Exception: Flags must contain only one of CALL_TOSTRING, TOSTRING_USE_KEY, TOSTRING_USE_CURRENT, TOSTRING_USE_INNER
int(0)
===6===
Exception: Flags must contain only one of CALL_TOSTRING, TOSTRING_USE_KEY, TOSTRING_USE_CURRENT, TOSTRING_USE_INNER
int(0)
===10===
Exception: Flags must contain only one of CALL_TOSTRING, TOSTRING_USE_KEY, TOSTRING_USE_CURRENT, TOSTRING_USE_INNER
int(0)
===X===
Exception: Unsetting flag CALL_TO_STRING is not possible
Exception: Unsetting flag TOSTRING_USE_INNER is not possible
===DONE===
