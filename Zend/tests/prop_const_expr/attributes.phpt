--TEST--
Allow fetching properties in attributes
--EXTENSIONS--
reflection
--FILE--
<?php

enum A: string {
    case B = 'C';
}

#[Attribute(Attribute::TARGET_CLASS | Attribute::IS_REPEATABLE)]
class Attr {
    public function __construct(public $value) {}
}

#[Attr(A::B->name)]
#[Attr(A::B->value)]
#[Attr(A::B?->name)]
#[Attr(A::B?->value)]
class C {}

foreach ((new ReflectionClass(C::class))->getAttributes() as $reflectionAttribute) {
    var_dump($reflectionAttribute->newInstance());
}

?>
--EXPECTF--
object(Attr)#%d (1) {
  ["value"]=>
  string(1) "B"
}
object(Attr)#%d (1) {
  ["value"]=>
  string(1) "C"
}
object(Attr)#%d (1) {
  ["value"]=>
  string(1) "B"
}
object(Attr)#%d (1) {
  ["value"]=>
  string(1) "C"
}
