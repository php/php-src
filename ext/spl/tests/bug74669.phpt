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

    public function current()
    {
        return $this->iterator->current();
    }

    public function next()
    {
        $this->iterator->next();
    }

    public function key()
    {
        return $this->iterator->key();
    }

    public function valid()
    {
        return $this->iterator->valid();
    }

    public function rewind()
    {
        $this->iterator->rewind();
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

?>
--EXPECTF--
0 => test1
1 => test2
0 => test1
1 => test2

Notice: Undefined index: foo in %s on line %s
NULL

