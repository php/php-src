--TEST--
SPL: ArrayObject/Iterator and reference to self
--FILE--
==ArrayObject===
<?php

class MyArrayObject extends ArrayObject
{
    public function __construct()
    {
        parent::__construct($this);
        $this['bar'] = 'baz';
    }
}

$a = new MyArrayObject;

$b = clone $a;
$b['baz'] = 'Foo';

var_dump($a);
var_dump($b);

?>
==ArrayIterator===
<?php

class MyArrayIterator extends ArrayIterator
{
    public function __construct()
    {
        parent::__construct($this);
        $this['bar'] = 'baz';
    }
}

$a = new MyArrayIterator;

$b = clone $a;
$b['baz'] = 'Foo';

var_dump($a);
var_dump($b);

?>
--EXPECTF--
==ArrayObject===

Deprecated: ArrayObject::__construct(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d
object(MyArrayObject)#%d (1) {
  ["bar"]=>
  string(3) "baz"
}
object(MyArrayObject)#%d (2) {
  ["bar"]=>
  string(3) "baz"
  ["baz"]=>
  string(3) "Foo"
}
==ArrayIterator===

Deprecated: ArrayIterator::__construct(): Using an object as a backing array for ArrayIterator is deprecated, as it allows violating class constraints and invariants in %s on line %d
object(MyArrayIterator)#%d (1) {
  ["bar"]=>
  string(3) "baz"
}
object(MyArrayIterator)#%d (2) {
  ["bar"]=>
  string(3) "baz"
  ["baz"]=>
  string(3) "Foo"
}
