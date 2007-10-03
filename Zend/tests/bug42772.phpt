--TEST--
Bug #42772 (Storing $this in a static var fails while handling a cast to string)
--FILE--
<?php
class Foo {
    static public $foo;
    function __toString() {
        self::$foo = $this;
        return 'foo';
    }
}

$foo = (string)new Foo();
var_dump(Foo::$foo);
?>
--EXPECT--
object(Foo)#1 (0) {
}
