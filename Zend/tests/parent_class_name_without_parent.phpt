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
Fatal error: Cannot use "parent" when current class scope has no parent in %s on line 5

