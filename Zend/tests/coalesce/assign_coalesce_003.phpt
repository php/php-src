--TEST--
Coalesce assign (??=): ArrayAccess handling
--FILE--
<?php

function id($arg) {
    echo "id($arg)\n";
    return $arg;
}

class AA implements ArrayAccess {
    public $data;
    public function __construct($data = []) {
        $this->data = $data;
    }
    public function &offsetGet($k): mixed {
        echo "offsetGet($k)\n";
        return $this->data[$k];
    }
    public function offsetExists($k): bool {
        echo "offsetExists($k)\n";
        return array_key_exists($k, $this->data);
    }
    public function offsetSet($k,$v): void {
        echo "offsetSet($k,$v)\n";
        $this->data[$k] = $v;
    }
    public function offsetUnset($k): void { }
}

$ary = new AA(["foo" => new AA, "null" => null]);

echo "[foo]\n";
$ary["foo"] ??= "bar";

echo "[bar]\n";
$ary["bar"] ??= "foo";

echo "[null]\n";
$ary["null"] ??= "baz";

echo "[foo][bar]\n";
$ary["foo"]["bar"] ??= "abc";

echo "[foo][bar]\n";
$ary["foo"]["bar"] ??= "def";

?>
--EXPECT--
[foo]
offsetExists(foo)
offsetGet(foo)
[bar]
offsetExists(bar)
offsetSet(bar,foo)
[null]
offsetExists(null)
offsetGet(null)
offsetSet(null,baz)
[foo][bar]
offsetExists(foo)
offsetGet(foo)
offsetExists(bar)
offsetGet(foo)
offsetSet(bar,abc)
[foo][bar]
offsetExists(foo)
offsetGet(foo)
offsetExists(bar)
offsetGet(bar)
