--TEST--
Bug #64966 (segfault in zend_do_fcall_common_helper_SPEC)
--FILE--
<?php
error_reporting(E_ALL);
set_error_handler(function($error) { throw new Exception(); }, E_RECOVERABLE_ERROR);

function test($func) {
	$a = $func("");
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
Fatal error: Uncaught exception 'Exception' in %sbug64966.php:3
Stack trace:
#0 [internal function]: {closure}(4096, 'Argument 1 pass...', '%s', 6, Array)
#1 %sbug64966.php(6): iterator_apply('')
#2 %sbug64966.php(12): test('iterator_apply')
#3 %sbug64966.php(17): A->b()
#4 {main}
  thrown in %sbug64966.php on line 3
