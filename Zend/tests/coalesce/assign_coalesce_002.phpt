--TEST--
Coalesce assign (??=): Exception handling
--FILE--
<?php

$foo = "fo";
$foo .= "o";
$bar = "ba";
$bar .= "r";

function id($arg) {
    echo "id($arg)\n";
    return $arg;
}

function do_throw($msg) {
    throw new Exception($msg);
}

$ary = [];
try {
    $ary[id($foo)] ??= do_throw("ex1");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($ary);

class AA implements ArrayAccess {
    public function offsetExists($k): bool {
        return true;
    }
    public function &offsetGet($k): mixed {
        $var = ["foo" => "bar"];
        return $var;
    }
    public function offsetSet($k,$v): void {}
    public function offsetUnset($k): void {}
}

class Dtor {
    public function __destruct() {
        throw new Exception("dtor");
    }
}

$ary = new AA;
try {
    $ary[new Dtor][id($foo)] ??= $bar;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($foo);

class AA2 implements ArrayAccess {
    public function offsetExists($k): bool {
        return false;
    }
    public function offsetGet($k): mixed {
        return null;
    }
    public function offsetSet($k,$v): void {}
    public function offsetUnset($k): void {}
}

$ary = ["foo" => new AA2];
try {
    $ary[id($foo)][new Dtor] ??= $bar;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($foo);

?>
--EXPECT--
id(foo)
Exception: ex1
array(0) {
}
id(foo)
Exception: dtor
string(3) "foo"
id(foo)
Exception: dtor
string(3) "foo"
