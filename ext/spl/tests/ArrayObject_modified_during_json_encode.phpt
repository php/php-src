--TEST--
Modifying an ArrayObject or ArrayIterator from within json_encode()
--FILE--
<?php

class Mutator implements JsonSerializable
{
    public function __construct(private object $target) {}

    public function jsonSerialize(): mixed
    {
        $this->target['b'] = 'mutated';
        for ($i = 0; $i < 64; $i++) {
            $this->target["appended$i"] = 'appended';
        }
        $ref = &$this->target['by-reference'];
        $ref = 'by-reference';
        return 'serialized';
    }
}

$self = new ArrayObject();
$self->exchangeArray($self);

$containers = [
    'array-backed ArrayObject' => new ArrayObject(),
    'array-backed ArrayIterator' => new ArrayIterator(),
    'self-backed ArrayObject' => $self,
    'object-backed ArrayObject' => new ArrayObject(new stdClass()),
];

foreach ($containers as $label => $container) {
    $container['a'] = null;
    $container['b'] = 'second';
    $container['c'] = 'third';
    $container['a'] = new Mutator($container);

    echo $label, ': ', json_encode($container), "\n";
    echo 'count: ', count($container), "\n";
}

?>
--EXPECT--
array-backed ArrayObject: {"a":"serialized","b":"second","c":"third"}
count: 68
array-backed ArrayIterator: {"a":"serialized","b":"second","c":"third"}
count: 68
self-backed ArrayObject: {"a":"serialized","b":"second","c":"third"}
count: 68
object-backed ArrayObject: {"a":"serialized","b":"second","c":"third"}
count: 68
