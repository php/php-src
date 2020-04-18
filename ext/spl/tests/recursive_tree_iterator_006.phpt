--TEST--
SPL: RecursiveTreeIterator and IteratorAggregate
--FILE--
<?php

$ary = array(
    0 => array(
        "a",
        1,
    ),
    "a" => array(
        2,
        "b",
        3 => array(
            4,
            "c",
        ),
        "3" => array(
            4,
            "c",
        ),
    ),
);

class RecursiveArrayIteratorAggregated implements IteratorAggregate {
    public $it;
    function __construct($it) {
        $this->it = new RecursiveArrayIterator($it);
    }
    function getIterator() {
        return $this->it;
    }
}

$it = new RecursiveArrayIteratorAggregated($ary);
echo "-- flags = BYPASS_KEY --\n";
foreach(new RecursiveTreeIterator($it) as $k => $v) {
    echo "[$k] => $v\n";
}
echo "-- flags = BYPASS_CURRENT --\n";
foreach(new RecursiveTreeIterator($it, RecursiveTreeIterator::BYPASS_CURRENT) as $k => $v) {
    echo "[$k] => $v\n";
}
echo "-- flags = BYPASS_KEY|BYPASS_KEY --\n";
foreach(new RecursiveTreeIterator($it, RecursiveTreeIterator::BYPASS_CURRENT|RecursiveTreeIterator::BYPASS_KEY) as $k => $v) {
    echo "[$k] => $v\n";
}
echo "-- flags = 0 --\n";
foreach(new RecursiveTreeIterator($it, 0) as $k => $v) {
    echo "[$k] => $v\n";
}
echo "-- flags = 0, caching_it_flags = CachingIterator::CATCH_GET_CHILD --\n";
foreach(new RecursiveTreeIterator($it, 0, CachingIterator::CATCH_GET_CHILD) as $k => $v) {
    echo "[$k] => $v\n";
}

?>
--EXPECTF--
-- flags = BYPASS_KEY --
[0] => |-Array
[0] => | |-a
[1] => | \-1
[a] => \-Array
[0] =>   |-2
[1] =>   |-b
[3] =>   \-Array
[0] =>     |-4
[1] =>     \-c
-- flags = BYPASS_CURRENT --

Warning: Array to string conversion in %s on line %d
[|-0] => Array
[| |-0] => a
[| \-1] => 1

Warning: Array to string conversion in %s on line %d
[\-a] => Array
[  |-0] => 2
[  |-1] => b

Warning: Array to string conversion in %s on line %d
[  \-3] => Array
[    |-0] => 4
[    \-1] => c
-- flags = BYPASS_KEY|BYPASS_KEY --

Warning: Array to string conversion in %s on line %d
[0] => Array
[0] => a
[1] => 1

Warning: Array to string conversion in %s on line %d
[a] => Array
[0] => 2
[1] => b

Warning: Array to string conversion in %s on line %d
[3] => Array
[0] => 4
[1] => c
-- flags = 0 --
[|-0] => |-Array
[| |-0] => | |-a
[| \-1] => | \-1
[\-a] => \-Array
[  |-0] =>   |-2
[  |-1] =>   |-b
[  \-3] =>   \-Array
[    |-0] =>     |-4
[    \-1] =>     \-c
-- flags = 0, caching_it_flags = CachingIterator::CATCH_GET_CHILD --
[|-0] => |-Array
[| |-0] => | |-a
[| \-1] => | \-1
[\-a] => \-Array
[  |-0] =>   |-2
[  |-1] =>   |-b
[  \-3] =>   \-Array
[    |-0] =>     |-4
[    \-1] =>     \-c
