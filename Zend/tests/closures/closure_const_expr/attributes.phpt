--TEST--
Allow defining closures in attributes
--EXTENSIONS--
reflection
--FILE--
<?php

#[Attribute(Attribute::TARGET_CLASS | Attribute::IS_REPEATABLE)]
class Attr {
    public function __construct(public Closure $value) {
        $value('foo');
    }
}

#[Attr(static function () { })]
#[Attr(static function (...$args) {
    var_dump($args);
})]
class C {}

foreach ((new ReflectionClass(C::class))->getAttributes() as $reflectionAttribute) {
    var_dump($reflectionAttribute->newInstance());
}

?>
--EXPECTF--
object(Attr)#%d (1) {
  ["value"]=>
  object(Closure)#%d (3) {
    ["name"]=>
    string(%d) "{closure:%s:%d}"
    ["file"]=>
    string(%d) "%s"
    ["line"]=>
    int(%d)
  }
}
array(1) {
  [0]=>
  string(3) "foo"
}
object(Attr)#%d (1) {
  ["value"]=>
  object(Closure)#%d (4) {
    ["name"]=>
    string(%d) "{closure:%s:%d}"
    ["file"]=>
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["parameter"]=>
    array(1) {
      ["$args"]=>
      string(10) "<optional>"
    }
  }
}
