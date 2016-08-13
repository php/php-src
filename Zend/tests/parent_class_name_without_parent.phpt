--TEST--
Getting parent class name when there is no parent generates an error
--FILE--
<?php

trait T {
    public function f() {
        var_dump(parent::class);
    }
}

class C {
    use T;
}

(new C)->f();

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot use "parent" when current class scope has no parent in %s:5
Stack trace:
#0 %s(%d): C->f()
#1 {main}
  thrown in %s on line 5

