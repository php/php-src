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

// test to check unserialize of old format
$oldFormat = 'C:11:"ArrayObject":76:{x:i:0;C:11:"ArrayObject":37:{x:i:0;a:2:{i:0;i:1;i:1;i:2;};m:a:0:{}};m:a:0:{}}';
$obj = unserialize($oldFormat);
print_r($obj);

?>
--EXPECT--
0 => test1
1 => test2
ArrayObject Object
(
    [storage:ArrayObject:private] => ArrayObject Object
        (
            [storage:ArrayObject:private] => Array
                (
                    [0] => 1
                    [1] => 2
                )

        )

)
