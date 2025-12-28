--TEST--
GH-20435: ZEND_CALL_HAS_EXTRA_NAMED_PARAMS & magic methods in debug_backtrace_get_args()
--FILE--
<?php

class C {
    public static function __callStatic($name, $args) {
        echo (new Exception())->__toString(), "\n\n";
    }
    public function __call($name, $args) {
        echo (new Exception())->__toString(), "\n\n";
    }
    public function __invoke(...$args) {
        echo (new Exception())->__toString(), "\n";
    }
}

$c = new C();
$c->foo(bar: 'bar');
C::foo(bar: 'bar');
$c(bar: 'bar');

?>
--EXPECTF--
Exception in %s:%d
Stack trace:
#0 %s(%d): C->__call('foo', Array)
#1 {main}

Exception in %s:%d
Stack trace:
#0 %s(%d): C::__callStatic('foo', Array)
#1 {main}

Exception in %s:%d
Stack trace:
#0 %s(%d): C->__invoke(bar: 'bar')
#1 {main}
