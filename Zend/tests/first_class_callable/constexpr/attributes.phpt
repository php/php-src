--TEST--
Allow defining FCC in attributes
--EXTENSIONS--
reflection
--FILE--
<?php

#[Attribute(Attribute::TARGET_CLASS | Attribute::IS_REPEATABLE)]
class Attr {
    public function __construct(public Closure $value) {
        var_dump($value('abc'));
    }
}

#[Attr(strrev(...))]
#[Attr(strlen(...))]
class C {}

foreach ((new ReflectionClass(C::class))->getAttributes() as $reflectionAttribute) {
    var_dump($reflectionAttribute->newInstance());
}

?>
--EXPECTF--
string(3) "cba"
object(Attr)#%d (1) {
  ["value"]=>
  object(Closure)#%d (2) {
    ["function"]=>
    string(6) "strrev"
    ["parameter"]=>
    array(1) {
      ["$string"]=>
      string(10) "<required>"
    }
  }
}
int(3)
object(Attr)#%d (1) {
  ["value"]=>
  object(Closure)#%d (2) {
    ["function"]=>
    string(6) "strlen"
    ["parameter"]=>
    array(1) {
      ["$string"]=>
      string(10) "<required>"
    }
  }
}
