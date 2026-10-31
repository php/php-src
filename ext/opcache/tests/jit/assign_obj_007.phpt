--TEST--
JIT ASSIGN_OBJ: exception thrown by a set hook on a typed property
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.jit=tracing
opcache.jit_hot_loop=1
opcache.jit_hot_func=1
opcache.jit_hot_return=1
opcache.jit_hot_side_exit=1
--FILE--
<?php
class Foo {
    public int $test = 1 {
        set(int $v) {
            if ($v === 3) {
                throw new Exception('hook threw');
            }
            $this->test = $v;
        }
    }
}

function test() {
    $foo = new Foo;
    for ($i = 0; $i < 10; $i++) {
        $foo->test = $i;
        echo $i, PHP_EOL;
    }
}

try {
    test();
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
0
1
2
Exception: hook threw
