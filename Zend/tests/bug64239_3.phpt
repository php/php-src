--TEST--
Bug #64239 (debug_print_backtrace() changed behavior)
--FILE--
<?php
class A {
	use T2 { t2method as Bmethod; }
}

class C extends A {
	public function Bmethod() {
		debug_print_backtrace(DEBUG_BACKTRACE_IGNORE_ARGS, 1);
    }
}

trait T2 {
	public function t2method() {
		debug_print_backtrace(DEBUG_BACKTRACE_IGNORE_ARGS, 1);
	}
}

$a = new A();
$a->Bmethod();
$a->t2method();

$c = new C();
$c->Bmethod();
$c->t2method();
?>
--EXPECTF--
#0  A->Bmethod() called at [%sbug64239_3.php:%d]
#0  A->t2method() called at [%sbug64239_3.php:%d]
#0  C->Bmethod() called at [%sbug64239_3.php:%d]
#0  A->t2method() called at [%sbug64239_3.php:%d]
