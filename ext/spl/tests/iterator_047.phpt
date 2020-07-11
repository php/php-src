--TEST--
SPL: RecursiveCachingIterator and exception in has/getChildren
--FILE--
<?php

class MyRecursiveArrayIterator extends RecursiveArrayIterator
{
    static public $fail = 0;

    static function fail($state, $method)
    {
        if (self::$fail == $state)
        {
            throw new Exception("State $state: $method()");
        }
    }

    function hasChildren()
    {
        echo __METHOD__ . "()\n";
        self::fail(1, __METHOD__);
        return parent::hasChildren();
    }

    function getChildren()
    {
        echo __METHOD__ . "()\n";
        self::fail(2, __METHOD__);
        return parent::getChildren();
    }
}

class MyRecursiveCachingIterator extends RecursiveCachingIterator
{
    function show()
    {
        MyRecursiveArrayIterator::$fail = 0;
        while(MyRecursiveArrayIterator::$fail < 4)
        {
            echo "===" . MyRecursiveArrayIterator::$fail . "===\n";
            try
            {
                foreach(new RecursiveIteratorIterator($this) as $k => $v)
                {
                    var_dump($k);
                    var_dump($v);
                }
            }
            catch (Exception $e)
            {
                echo "Exception: " . $e->getMessage() . " in " . $e->getFile() . " on line " . $e->getLine() . "\n";
            }
            MyRecursiveArrayIterator::$fail++;
        }
    }
}

$it = new MyRecursiveArrayIterator(array(0, array(10), 2, array(30), 4));
$it = new MyRecursiveCachingIterator($it);

$it->show();

?>
--EXPECTF--
===0===
MyRecursiveArrayIterator::hasChildren()
int(0)
int(0)
MyRecursiveArrayIterator::hasChildren()
MyRecursiveArrayIterator::getChildren()

Warning: Array to string conversion in %s on line %d
MyRecursiveArrayIterator::hasChildren()
int(0)
int(10)
MyRecursiveArrayIterator::hasChildren()
int(2)
int(2)
MyRecursiveArrayIterator::hasChildren()
MyRecursiveArrayIterator::getChildren()

Warning: Array to string conversion in %s on line %d
MyRecursiveArrayIterator::hasChildren()
int(0)
int(30)
MyRecursiveArrayIterator::hasChildren()
int(4)
int(4)
===1===
MyRecursiveArrayIterator::hasChildren()
Exception: State 1: MyRecursiveArrayIterator::hasChildren() in %s on line %d
===2===
MyRecursiveArrayIterator::hasChildren()
int(0)
int(0)
MyRecursiveArrayIterator::hasChildren()
MyRecursiveArrayIterator::getChildren()
Exception: State 2: MyRecursiveArrayIterator::getChildren() in %s on line %d
===3===
MyRecursiveArrayIterator::hasChildren()
int(0)
int(0)
MyRecursiveArrayIterator::hasChildren()
MyRecursiveArrayIterator::getChildren()

Warning: Array to string conversion in %s on line %d
MyRecursiveArrayIterator::hasChildren()
int(0)
int(10)
MyRecursiveArrayIterator::hasChildren()
int(2)
int(2)
MyRecursiveArrayIterator::hasChildren()
MyRecursiveArrayIterator::getChildren()

Warning: Array to string conversion in %s on line %d
MyRecursiveArrayIterator::hasChildren()
int(0)
int(30)
MyRecursiveArrayIterator::hasChildren()
int(4)
int(4)
