--TEST--
Assign coalesce: All calls should be memoized
--FILE--
<?php
class Foo {
    public $prop;

    public function foo() {
        echo __METHOD__, "\n";
        return $this;
    }

    public function bar() {
        echo __METHOD__, "\n";
        return 'prop';
    }

    public function __isset($name) {
        echo __METHOD__, "\n";
        return false;
    }

    public function __set($name, $value) {
        echo __METHOD__, "\n";
        var_dump($value);
    }
}

function &foo() {
    global $foo;
    echo __FUNCTION__, "\n";
    return $foo;
}
function bar() {
    echo __FUNCTION__, "\n";
}

foo(bar())['bar'] ??= 42;
var_dump($foo);

$foo = new Foo();
$foo->foo()->foo()->{$foo->bar()} ??= 42;
var_dump($foo);
$foo->foo()->baz ??= 42;

?>
--EXPECT--
bar
foo
array(1) {
  ["bar"]=>
  int(42)
}
Foo::foo
Foo::foo
Foo::bar
object(Foo)#1 (1) {
  ["prop"]=>
  int(42)
}
Foo::foo
Foo::__isset
Foo::__set
int(42)
