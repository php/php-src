--TEST--
Bug #74669: Unserialize ArrayIterator broken
--FILE--
<?php

class Container implements Iterator
{
    public $container;
    public $iterator;

    public function __construct()
    {
        $this->container = new ArrayObject();
        $this->iterator = $this->container->getIterator();
    }

    public function append($element)
    {
        $this->container->append($element);
    }

    public function current(): mixed
    {
        return $this->iterator->current();
    }

    public function next(): void
    {
        $this->iterator->next();
    }

    public function key(): mixed
    {
        return $this->iterator->key();
    }

    public function valid(): bool
    {
        return $this->iterator->valid();
    }

    public function rewind(): void
    {
        $this->iterator->rewind();
    }
}

class SelfArray extends ArrayObject
{
    public function __construct()
    {
        parent::__construct($this);
    }
}

$container = new Container();
$container->append('test1');
$container->append('test2');
$container->valid();
$serialized = serialize($container);
unset($container);

$container = unserialize($serialized);

foreach ($container as $key => $value) {
    echo $key . ' => ' . $value . PHP_EOL;
}

$arObj = new ArrayObject(['test1', 'test2']);
$serialized = serialize($container);
unset($arObj);

$arObj = unserialize($serialized);
foreach($arObj as $key => $value) {
    echo $key . ' => ' . $value . PHP_EOL;
}

$payload = 'x:i:33554432;O:8:"stdClass":0:{};m:a:0:{}';
$str = 'C:11:"ArrayObject":' . strlen($payload) . ':{' . $payload . '}';

$ao = unserialize($str);
var_dump($ao['foo']);

$selfArray = new SelfArray();
$selfArray['foo'] = 'bar';
var_dump($selfArray);
$serialized = serialize($selfArray);
var_dump($serialized);
unset($selfArray);
$selfArray = unserialize($serialized);
var_dump($selfArray);
var_dump($selfArray['foo']);

?>
--EXPECTF--
Deprecated: ArrayIterator::__unserialize(): Using an object as a backing array for ArrayIterator is deprecated, as it allows violating class constraints and invariants in %s on line %d
0 => test1
1 => test2

Deprecated: ArrayIterator::__unserialize(): Using an object as a backing array for ArrayIterator is deprecated, as it allows violating class constraints and invariants in %s on line %d
0 => test1
1 => test2

Deprecated: ArrayObject::unserialize(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d

Warning: Undefined array key "foo" in %s on line %d
NULL

Deprecated: ArrayObject::__construct(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d
object(SelfArray)#9 (1) {
  ["foo"]=>
  string(3) "bar"
}
string(77) "O:9:"SelfArray":4:{i:0;i:16777216;i:1;N;i:2;a:1:{s:3:"foo";s:3:"bar";}i:3;N;}"

Deprecated: Creation of dynamic property SelfArray::$foo is deprecated in %s on line %d
object(SelfArray)#9 (1) {
  ["foo"]=>
  string(3) "bar"
}
string(3) "bar"
