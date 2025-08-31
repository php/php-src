--TEST--
SPL: CachingIterator and offsetGet/Exists using flag FULL_CACHE
--FILE--
<?php

class MyFoo
{
    function __toString()
    {
        return 'foo';
    }
}

class MyCachingIterator extends CachingIterator
{
    function __construct(Iterator $it, $flags = 0)
    {
        parent::__construct($it, $flags);
    }

    function test($ar)
    {
        foreach($ar as $k => $v)
        {
            echo "===$k===\n";
            var_dump($v);
            try {
                var_dump($this->offsetExists($v));
            } catch (TypeError $e) {
                echo $e->getMessage(), "\n";
            }
            try {
                var_dump($this->offsetGet($v));
            } catch (TypeError $e) {
                echo $e->getMessage(), "\n";
            }
        }
    }
}

$it = new MyCachingIterator(new ArrayIterator(array(0, 'foo'=>1, 2, 'bar'=>3, 4)));

try
{
    var_dump($it->offsetExists(0));
}
catch(Exception $e)
{
    echo "Exception: " . $e->getMessage() . "\n";
}

try
{
    var_dump($it->offsetGet(0));
}
catch(Exception $e)
{
    echo "Exception: " . $e->getMessage() . "\n";
}

$it = new MyCachingIterator(new ArrayIterator(array(0, 'foo'=>1, 2, 'bar'=>3, 4)), CachingIterator::FULL_CACHE);

$checks = array(0, new stdClass, new MyFoo, NULL, 2, 'foo', 3);

$it->test($checks);

echo "===FILL===\n";

foreach($it as $v); // read all into cache

$it->test($checks);

?>
--EXPECTF--
Exception: MyCachingIterator does not use a full cache (see CachingIterator::__construct)
Exception: MyCachingIterator does not use a full cache (see CachingIterator::__construct)
===0===
int(0)
bool(false)

Warning: Undefined array key "0" in %s on line %d
NULL
===1===
object(stdClass)#%d (0) {
}
CachingIterator::offsetExists(): Argument #1 ($key) must be of type string, stdClass given
CachingIterator::offsetGet(): Argument #1 ($key) must be of type string, stdClass given
===2===
object(MyFoo)#%d (0) {
}
bool(false)

Warning: Undefined array key "foo" in %s on line %d
NULL
===3===
NULL

Deprecated: CachingIterator::offsetExists(): Passing null to parameter #1 ($key) of type string is deprecated in %s on line %d
bool(false)

Deprecated: CachingIterator::offsetGet(): Passing null to parameter #1 ($key) of type string is deprecated in %s on line %d

Warning: Undefined array key "" in %s on line %d
NULL
===4===
int(2)
bool(false)

Warning: Undefined array key "2" in %s on line %d
NULL
===5===
string(3) "foo"
bool(false)

Warning: Undefined array key "foo" in %s on line %d
NULL
===6===
int(3)
bool(false)

Warning: Undefined array key "3" in %s on line %d
NULL
===FILL===
===0===
int(0)
bool(true)
int(0)
===1===
object(stdClass)#1 (0) {
}
CachingIterator::offsetExists(): Argument #1 ($key) must be of type string, stdClass given
CachingIterator::offsetGet(): Argument #1 ($key) must be of type string, stdClass given
===2===
object(MyFoo)#2 (0) {
}
bool(true)
int(1)
===3===
NULL

Deprecated: CachingIterator::offsetExists(): Passing null to parameter #1 ($key) of type string is deprecated in %s on line %d
bool(false)

Deprecated: CachingIterator::offsetGet(): Passing null to parameter #1 ($key) of type string is deprecated in %s on line %d

Warning: Undefined array key "" in %s on line %d
NULL
===4===
int(2)
bool(true)
int(4)
===5===
string(3) "foo"
bool(true)
int(1)
===6===
int(3)
bool(false)

Warning: Undefined array key "3" in %s on line %d
NULL
