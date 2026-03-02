--TEST--
Bug #64239 (debug_backtrace() changed behavior)
--FILE--
<?php
class A {
    use T1;
    public function test() { $this->backtrace(); }
}

class B {
    use T2 { t2method as Bmethod; }
}

class C extends A {
}

trait T1 {
    protected function backtrace() {
        $b = new B();
        $b->Bmethod();
    }
}
trait T2 {
    public function t2method() {
        print_r(debug_backtrace(DEBUG_BACKTRACE_IGNORE_ARGS, 1));
    }
}
$a = new A();
$a->test();

$c = new C();
$c->test();
?>
--EXPECTF--
Array
(
    [0] => Array
        (
            [file] => %sbug64239_2.php
            [line] => %d
            [function] => Bmethod
            [class] => B
            [type] => ->
        )

)
Array
(
    [0] => Array
        (
            [file] => %sbug64239_2.php
            [line] => %d
            [function] => Bmethod
            [class] => B
            [type] => ->
        )

)
