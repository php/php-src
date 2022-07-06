--TEST--
Bug #81255: Memory leak in PHPUnit with functional JIT
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=1M
opcache.jit=function
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
eval('class B {}');
class A extends B {
    private ?string $x = null;

    public function foo($a) {
        if (!($this->x = str_repeat($a, 5))) {
	        throw new Exception('ops');
        }
        var_dump($this->x);
        $this->x = null;
    }
}

$a = new A;
$a->foo('a');
$a->foo('b');
?>
--EXPECT--
string(5) "aaaaa"
string(5) "bbbbb"
