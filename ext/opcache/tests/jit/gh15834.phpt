--TEST--
GH-15834 (Segfault with hook "simple get" cache slot and minimal JIT)
--EXTENSIONS--
opcache
--INI--
opcache.jit=1111
--FILE--
<?php
class A {
    public $_prop = 1;
    public $prop {
        get => $this->_prop;
    }
}

$a = new A;
for ($i=0;$i<5;$i++) {
    echo $a->prop;
    $a->_prop++;
}
?>
--EXPECT--
12345
