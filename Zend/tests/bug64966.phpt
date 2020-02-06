--TEST--
Bug #64966 (segfault in zend_do_fcall_common_helper_SPEC)
--FILE--
<?php
function test($func) {
    try {
        $a = $func("");
    } catch (Error $e) {
        throw new Exception();
    }
    return true;
}
class A {
    public function b() {
        test("strlen");
        test("iterator_apply");
    }
}

$a = new A();
$a->b();
?>
--EXPECTF--
Fatal error: Uncaught Exception in %sbug64966.php:6
Stack trace:
#0 %sbug64966.php(13): test('iterator_apply')
#1 %sbug64966.php(18): A->b()
#2 {main}
  thrown in %sbug64966.php on line 6
