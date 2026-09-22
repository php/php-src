--TEST--
GH-22158 (Tracing JIT dispatches observer begin handler through the wrong run_time_cache slot on megamorphic calls)
--EXTENSIONS--
opcache
zend_test
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=32M
opcache.jit_max_polymorphic_calls=0
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_output=0
zend_test.observer.reserve_op_array_handle=1
--FILE--
<?php
interface S { public function f(): int; }
final class A implements S { public function f(): int { return 1; } }
final class B implements S { public function f(): int { return 2; } }
final class C implements S { public function f(): int { return 3; } }
final class D implements S { public function f(): int { return 4; } }
final class E implements S { public function f(): int { return 5; } }

$o = [new A, new B, new C, new D, new E];
$t = 0;
for ($i = 0; $i < 200000; $i++) {
    $t += $o[$i % 5]->f();
}
echo $t, "\n";
?>
--EXPECT--
600000
