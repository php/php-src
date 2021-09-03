--TEST--
SPL: iterator_to_array() and exceptions from delayed destruct
--FILE--
<?php

class MyArrayIterator extends ArrayIterator
{
    static protected $fail = 0;
    public $state;

    static function fail($state, $method)
    {
        if (self::$fail == $state)
        {
            throw new Exception("State $state: $method()");
        }
    }

    function __construct()
    {
        $this->state = MyArrayIterator::$fail;
        self::fail(0, __FUNCTION__);
        parent::__construct(array(1, 2));
        self::fail(1, __FUNCTION__);
    }

    function rewind(): void
    {
        self::fail(2, __FUNCTION__);
        parent::rewind();
    }

    function valid(): bool
    {
        self::fail(3, __FUNCTION__);
        return parent::valid();
    }

    function current(): mixed
    {
        self::fail(4, __FUNCTION__);
        return parent::current();
    }

    function key(): string|int|null
    {
        self::fail(5, __FUNCTION__);
        return parent::key();
    }

    function next(): void
    {
        self::fail(6, __FUNCTION__);
        parent::next();
    }

    function __destruct()
    {
        self::fail(7, __FUNCTION__);
    }

    static function test($func, $skip = null)
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
                echo $e->getMessage() . "\n";
            }
            if (isset($skip[self::$fail]))
            {
                self::$fail = $skip[self::$fail];
            }
            else
            {
                self::$fail++;
            }
            try {
                $e = null;
            } catch (Exception $e) {
            }
        }
    }
}

MyArrayIterator::test('iterator_to_array');
MyArrayIterator::test('iterator_count', array(3 => 6));

?>
--EXPECT--
===iterator_to_array===
State 0: __construct()
State 1: __construct()
State 2: rewind()
State 3: valid()
State 4: current()
State 5: key()
State 6: next()
State 7: __destruct()
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
===iterator_count===
State 0: __construct()
State 1: __construct()
State 2: rewind()
State 3: valid()
State 6: next()
State 7: __destruct()
int(2)
