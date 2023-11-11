--TEST--
GH-12380: JIT+private array property access inside closure accesses private property in child class
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.protect_memory=1
opcache.jit=tracing
opcache.jit_hot_loop=1
opcache.jit_hot_func=1
opcache.jit_hot_return=1
opcache.jit_hot_side_exit=1
--EXTENSIONS--
opcache
--FILE--
<?php

abstract class a
{
    private int $v = 1;

    public function test(): void
    {
        var_dump($this->v);
        (function (): void {
            var_dump($this->v);
        })();
    }
}

final class b extends a {
    private int $v = 0;
}
$a = new b;

for ($i = 0; $i < 10; $i++) {
    $a->test();
}

?>
--EXPECT--
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
