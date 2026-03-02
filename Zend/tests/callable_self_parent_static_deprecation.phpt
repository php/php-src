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
        $variants = [
            '"self::foo"'            => "self::foo",
            '"parent::foo"'          => "parent::foo",
            '"static::foo"'          => "static::foo",
            '["self", "foo"]'        => ["self", "foo"],
            '["parent", "foo"]'      => ["parent", "foo"],
            '["static", "foo"]'      => ["static", "foo"],
            '["B", "self::foo"]'     => ["B", "self::foo"],
            '["B", "parent::foo"]'   => ["B", "parent::foo"],
            '["B", "static::foo"]'   => ["B", "static::foo"],
            '["B", "A::foo"]'        => ["B", "A::foo"],
            '[$this, "self::foo"]'   => [$this, "self::foo"],
            '[$this, "parent::foo"]' => [$this, "parent::foo"],
            '[$this, "static::foo"]' => [$this, "static::foo"],
            '[$this, "A::foo"]'      => [$this, "A::foo"],
        ];

        echo "==> Test call_user_func\n";
        foreach ($variants as $description => $callable) {
            echo "$description\n";
            call_user_func($callable);
        }
        echo "\n==> Test call_user_func_array\n";
        foreach ($variants as $description => $callable) {
                    echo "$description\n";
            call_user_func_array($callable, []);
        }

        // Also applies to other things performing calls
        echo "\n==> Test array_map\n";
        foreach ($variants as $description => $callable) {
            echo "$description\n";
            array_map($callable, [1]);
        }

        echo "\n==> Test is_callable()\n";
        foreach ($variants as $description => $callable) {
            echo "$description\n";
            var_dump(is_callable($callable));
        }

        echo "\n==> Test callable type hint\n";
        foreach ($variants as $description => $callable) {
            echo "$description\n";
            $this->callableTypeHint($callable);
        }
    }

    public function callableTypeHint(callable $c) {}
}

$b = new B;
$b->test();

?>
--EXPECTF--
==> Test call_user_func
"self::foo"

Deprecated: Use of "self" in callables is deprecated in %s on line %d
"parent::foo"

Deprecated: Use of "parent" in callables is deprecated in %s on line %d
"static::foo"

Deprecated: Use of "static" in callables is deprecated in %s on line %d
["self", "foo"]

Deprecated: Use of "self" in callables is deprecated in %s on line %d
["parent", "foo"]

Deprecated: Use of "parent" in callables is deprecated in %s on line %d
["static", "foo"]

Deprecated: Use of "static" in callables is deprecated in %s on line %d
["B", "self::foo"]

Deprecated: Callables of the form ["B", "self::foo"] are deprecated in %s on line %d
["B", "parent::foo"]

Deprecated: Callables of the form ["B", "parent::foo"] are deprecated in %s on line %d
["B", "static::foo"]

Deprecated: Callables of the form ["B", "static::foo"] are deprecated in %s on line %d
["B", "A::foo"]

Deprecated: Callables of the form ["B", "A::foo"] are deprecated in %s on line %d
[$this, "self::foo"]

Deprecated: Callables of the form ["B", "self::foo"] are deprecated in %s on line %d
[$this, "parent::foo"]

Deprecated: Callables of the form ["B", "parent::foo"] are deprecated in %s on line %d
[$this, "static::foo"]

Deprecated: Callables of the form ["B", "static::foo"] are deprecated in %s on line %d
[$this, "A::foo"]

Deprecated: Callables of the form ["B", "A::foo"] are deprecated in %s on line %d

==> Test call_user_func_array
"self::foo"

Deprecated: Use of "self" in callables is deprecated in %s on line %d
"parent::foo"

Deprecated: Use of "parent" in callables is deprecated in %s on line %d
"static::foo"

Deprecated: Use of "static" in callables is deprecated in %s on line %d
["self", "foo"]

Deprecated: Use of "self" in callables is deprecated in %s on line %d
["parent", "foo"]

Deprecated: Use of "parent" in callables is deprecated in %s on line %d
["static", "foo"]

Deprecated: Use of "static" in callables is deprecated in %s on line %d
["B", "self::foo"]

Deprecated: Callables of the form ["B", "self::foo"] are deprecated in %s on line %d
["B", "parent::foo"]

Deprecated: Callables of the form ["B", "parent::foo"] are deprecated in %s on line %d
["B", "static::foo"]

Deprecated: Callables of the form ["B", "static::foo"] are deprecated in %s on line %d
["B", "A::foo"]

Deprecated: Callables of the form ["B", "A::foo"] are deprecated in %s on line %d
[$this, "self::foo"]

Deprecated: Callables of the form ["B", "self::foo"] are deprecated in %s on line %d
[$this, "parent::foo"]

Deprecated: Callables of the form ["B", "parent::foo"] are deprecated in %s on line %d
[$this, "static::foo"]

Deprecated: Callables of the form ["B", "static::foo"] are deprecated in %s on line %d
[$this, "A::foo"]

Deprecated: Callables of the form ["B", "A::foo"] are deprecated in %s on line %d

==> Test array_map
"self::foo"

Deprecated: Use of "self" in callables is deprecated in %s on line %d
"parent::foo"

Deprecated: Use of "parent" in callables is deprecated in %s on line %d
"static::foo"

Deprecated: Use of "static" in callables is deprecated in %s on line %d
["self", "foo"]

Deprecated: Use of "self" in callables is deprecated in %s on line %d
["parent", "foo"]

Deprecated: Use of "parent" in callables is deprecated in %s on line %d
["static", "foo"]

Deprecated: Use of "static" in callables is deprecated in %s on line %d
["B", "self::foo"]

Deprecated: Callables of the form ["B", "self::foo"] are deprecated in %s on line %d
["B", "parent::foo"]

Deprecated: Callables of the form ["B", "parent::foo"] are deprecated in %s on line %d
["B", "static::foo"]

Deprecated: Callables of the form ["B", "static::foo"] are deprecated in %s on line %d
["B", "A::foo"]

Deprecated: Callables of the form ["B", "A::foo"] are deprecated in %s on line %d
[$this, "self::foo"]

Deprecated: Callables of the form ["B", "self::foo"] are deprecated in %s on line %d
[$this, "parent::foo"]

Deprecated: Callables of the form ["B", "parent::foo"] are deprecated in %s on line %d
[$this, "static::foo"]

Deprecated: Callables of the form ["B", "static::foo"] are deprecated in %s on line %d
[$this, "A::foo"]

Deprecated: Callables of the form ["B", "A::foo"] are deprecated in %s on line %d

==> Test is_callable()
"self::foo"

Deprecated: Use of "self" in callables is deprecated in %s on line %d
bool(true)
"parent::foo"

Deprecated: Use of "parent" in callables is deprecated in %s on line %d
bool(true)
"static::foo"

Deprecated: Use of "static" in callables is deprecated in %s on line %d
bool(true)
["self", "foo"]

Deprecated: Use of "self" in callables is deprecated in %s on line %d
bool(true)
["parent", "foo"]

Deprecated: Use of "parent" in callables is deprecated in %s on line %d
bool(true)
["static", "foo"]

Deprecated: Use of "static" in callables is deprecated in %s on line %d
bool(true)
["B", "self::foo"]

Deprecated: Callables of the form ["B", "self::foo"] are deprecated in %s on line %d
bool(true)
["B", "parent::foo"]

Deprecated: Callables of the form ["B", "parent::foo"] are deprecated in %s on line %d
bool(true)
["B", "static::foo"]

Deprecated: Callables of the form ["B", "static::foo"] are deprecated in %s on line %d
bool(true)
["B", "A::foo"]

Deprecated: Callables of the form ["B", "A::foo"] are deprecated in %s on line %d
bool(true)
[$this, "self::foo"]

Deprecated: Callables of the form ["B", "self::foo"] are deprecated in %s on line %d
bool(true)
[$this, "parent::foo"]

Deprecated: Callables of the form ["B", "parent::foo"] are deprecated in %s on line %d
bool(true)
[$this, "static::foo"]

Deprecated: Callables of the form ["B", "static::foo"] are deprecated in %s on line %d
bool(true)
[$this, "A::foo"]

Deprecated: Callables of the form ["B", "A::foo"] are deprecated in %s on line %d
bool(true)

==> Test callable type hint
"self::foo"

Deprecated: Use of "self" in callables is deprecated in %s on line %d
"parent::foo"

Deprecated: Use of "parent" in callables is deprecated in %s on line %d
"static::foo"

Deprecated: Use of "static" in callables is deprecated in %s on line %d
["self", "foo"]

Deprecated: Use of "self" in callables is deprecated in %s on line %d
["parent", "foo"]

Deprecated: Use of "parent" in callables is deprecated in %s on line %d
["static", "foo"]

Deprecated: Use of "static" in callables is deprecated in %s on line %d
["B", "self::foo"]

Deprecated: Callables of the form ["B", "self::foo"] are deprecated in %s on line %d
["B", "parent::foo"]

Deprecated: Callables of the form ["B", "parent::foo"] are deprecated in %s on line %d
["B", "static::foo"]

Deprecated: Callables of the form ["B", "static::foo"] are deprecated in %s on line %d
["B", "A::foo"]

Deprecated: Callables of the form ["B", "A::foo"] are deprecated in %s on line %d
[$this, "self::foo"]

Deprecated: Callables of the form ["B", "self::foo"] are deprecated in %s on line %d
[$this, "parent::foo"]

Deprecated: Callables of the form ["B", "parent::foo"] are deprecated in %s on line %d
[$this, "static::foo"]

Deprecated: Callables of the form ["B", "static::foo"] are deprecated in %s on line %d
[$this, "A::foo"]

Deprecated: Callables of the form ["B", "A::foo"] are deprecated in %s on line %d
