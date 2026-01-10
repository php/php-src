--TEST--
GH-16186 001 (Non-tracing JIT uses Closure scope)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=64M
opcache.jit=0234
opcache.file_update_protection=0
opcache.revalidate_freq=0
opcache.protect_memory=1
--FILE--
<?php

class A {
    private $x;
    public function getIncrementor() {
        return function() { $this->x++; };
    }
}
$a = new A();
$f = $a->getIncrementor();
$c = new stdClass();
$f();
$f2 = Closure::bind($f, $c);
$f2();
$f2();

var_dump($c);

?>
--EXPECTF--
Warning: Undefined property: stdClass::$x in %s on line %d
object(stdClass)#%d (1) {
  ["x"]=>
  int(2)
}
