--TEST--
GH-17851: Use-after-free when instantiating autoloaded class with attribute having a Closure parameter.
--EXTENSIONS--
reflection
--FILE--
<?php

spl_autoload_register(static function ($className) {
    if ($className === 'C') {
        require(__DIR__ . '/attributes_autoload.inc');
    }
});

#[Attribute(Attribute::TARGET_CLASS | Attribute::IS_REPEATABLE)]
class Attr {
    public function __construct(public Closure $value) {
        $value('foo');
    }
}

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
