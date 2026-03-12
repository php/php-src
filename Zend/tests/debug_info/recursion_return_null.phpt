--TEST--
Testing __debugInfo() magic method
--FILE--
<?php

set_error_handler(
    static function () {
        echo "in handler\n";
        $f = new Foo();
        var_dump($f);
    }
);

class Foo {
  public function __debugInfo() {
    return null;
  }
}

$f = new Foo;
var_dump($f);

?>
--EXPECTF--
in handler

Deprecated: Returning null from Foo::__debugInfo() is deprecated, return an empty array instead in %s on line %d
object(Foo)#3 (0) {
}
object(Foo)#2 (0) {
}
