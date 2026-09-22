--TEST--
Array offset on ArrayAccess object in virtual property is allowed
--FILE--
<?php

class Collection implements ArrayAccess {
    public function offsetExists(mixed $offset): bool {
        echo __METHOD__ . "\n";
        return true;
    }

    public function offsetGet(mixed $offset): mixed {
        echo __METHOD__ . "\n";
        return true;
    }

    public function offsetSet(mixed $offset, mixed $value): void {
        echo __METHOD__ . "\n";
    }

    public function offsetUnset(mixed $offset): void {
        echo __METHOD__ . "\n";
    }
}

class C {
    public function __construct(
        public Collection $collection = new Collection(),
    ) {}
    public $prop {
        get => $this->collection;
    }
}

$c = new C();
var_dump($c->prop['foo']);
var_dump($c->prop[] = 'foo');
var_dump(isset($c->prop['foo']));
unset($c->prop['foo']);

?>
--EXPECT--
Collection::offsetGet
bool(true)
Collection::offsetSet
string(3) "foo"
Collection::offsetExists
bool(true)
Collection::offsetUnset
