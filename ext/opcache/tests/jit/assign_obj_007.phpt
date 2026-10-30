--TEST--
JIT: assigning to a typed property must not use the object after coercion user code released it
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=64M
opcache.jit=function
--EXTENSIONS--
opcache
--FILE--
<?php

class Test {
    public string $s = "a";
}

class Releaser {
    public function __toString(): string {
        unset($GLOBALS['a'][0]);
        return "5";
    }
}

function assign(array &$a, $v) {
    $a[0]->s = $v;
}

for ($i = 0; $i < 3; $i++) {
    $a = [new Test];
    try {
        assign($a, new Releaser);
        echo "assigned\n";
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
Object was released while assigning to property Test::$s
Object was released while assigning to property Test::$s
Object was released while assigning to property Test::$s
