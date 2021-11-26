--TEST--
Deprecation of self/parent/static in callables
--FILE--
<?php

class A {
    public function foo() {}
}
class B extends A {
    public function test() {
        // Different callables using self/parent/static
        echo "Test different callables\n";
        call_user_func("self::foo");
        call_user_func("parent::foo");
        call_user_func("static::foo");
        call_user_func(["self", "foo"]);
        call_user_func(["parent", "foo"]);
        call_user_func(["static", "foo"]);
        call_user_func(["B", "self::foo"]);
        call_user_func(["B", "parent::foo"]);
        call_user_func(["B", "static::foo"]);
        call_user_func(["B", "A::foo"]);

        // Also applies to other things performing calls
        echo "Test array_map()\n";
        array_map("self::foo", [1]);

        echo "Test is_callable() -- should be silent\n";
        var_dump(is_callable("self::foo"));

        echo "Test callable type hint -- should be silent\n";
        $this->callableTypeHint("self::foo");
    }

    public function callableTypeHint(callable $c) {}
}

$b = new B;
$b->test();

?>
--EXPECTF--
Test different callables

Deprecated: Use of "self" in callables is deprecated in %s on line %d

Deprecated: Use of "parent" in callables is deprecated in %s on line %d

Deprecated: Use of "static" in callables is deprecated in %s on line %d

Deprecated: Use of "self" in callables is deprecated in %s on line %d

Deprecated: Use of "parent" in callables is deprecated in %s on line %d

Deprecated: Use of "static" in callables is deprecated in %s on line %d

Deprecated: Callables of the form ["B", "self::foo"] are deprecated in %s on line %d

Deprecated: Callables of the form ["B", "parent::foo"] are deprecated in %s on line %d

Deprecated: Callables of the form ["B", "static::foo"] are deprecated in %s on line %d

Deprecated: Callables of the form ["B", "A::foo"] are deprecated in %s on line %d
Test array_map()

Deprecated: Use of "self" in callables is deprecated in %s on line %d
Test is_callable() -- should be silent
bool(true)
Test callable type hint -- should be silent
