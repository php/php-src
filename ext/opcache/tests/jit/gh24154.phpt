--TEST--
GH-24154 (Function JIT: ++/-- on a typed property holding a reference throws TypeError)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.jit=function
--EXTENSIONS--
opcache
--FILE--
<?php
class Test
{
    public int $value = 0;

    public function inc(): void
    {
        $this->value++;
    }

    public function dec(): void
    {
        $this->value--;
    }
}

$test = new Test();
$ref = &$test->value;
$test->inc();
var_dump($test->value);
$test->dec();
$test->dec();
var_dump($test->value, $ref);
?>
--EXPECT--
int(1)
int(-1)
int(-1)
