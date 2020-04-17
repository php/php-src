--TEST--
SPL: CachingIterator and __toString
--FILE--
<?php

function test($ar, $flags)
{
	echo "===$flags===\n";
	$it = new CachingIterator($ar, 0);
    try {
		$it->setFlags($flags);
    } catch (\ValueError $e) {
        echo $e->getMessage() . PHP_EOL;
		var_dump($it->getFlags());
		return;
    }
	var_dump($it->getFlags());
	try {
		foreach($it as $v) {
			var_dump((string)$it);
		}
	} catch (Exception $e) {
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
} catch (\ValueError $e) {
    echo get_class($e) . ': ' . $e->getMessage() . \PHP_EOL;
}
try {
    $it = new CachingIterator($ar, CachingIterator::TOSTRING_USE_INNER);
    $it->setFlags(0);
} catch (\ValueError $e) {
    echo get_class($e) . ': ' . $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
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
CachingIterator::setFlags(): Argument #1 ($flags) must contain only one of CALL_TOSTRING, TOSTRING_USE_KEY, TOSTRING_USE_CURRENT, TOSTRING_USE_INNER
int(0)
===5===
CachingIterator::setFlags(): Argument #1 ($flags) must contain only one of CALL_TOSTRING, TOSTRING_USE_KEY, TOSTRING_USE_CURRENT, TOSTRING_USE_INNER
int(0)
===9===
CachingIterator::setFlags(): Argument #1 ($flags) must contain only one of CALL_TOSTRING, TOSTRING_USE_KEY, TOSTRING_USE_CURRENT, TOSTRING_USE_INNER
int(0)
===6===
CachingIterator::setFlags(): Argument #1 ($flags) must contain only one of CALL_TOSTRING, TOSTRING_USE_KEY, TOSTRING_USE_CURRENT, TOSTRING_USE_INNER
int(0)
===10===
CachingIterator::setFlags(): Argument #1 ($flags) must contain only one of CALL_TOSTRING, TOSTRING_USE_KEY, TOSTRING_USE_CURRENT, TOSTRING_USE_INNER
int(0)
===X===
ValueError: CachingIterator::setFlags(): Argument #1 ($flags) cannot unset flag CALL_TO_STRING
ValueError: CachingIterator::setFlags(): Argument #1 ($flags) cannot unset flag TOSTRING_USE_INNER
