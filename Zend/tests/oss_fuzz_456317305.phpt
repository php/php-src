--TEST--
OSS-Fuzz #456317305: EG(current_execute_data) NULL pointer violation
--FILE--
<?php

class C {
    public function __destruct() {
        static $again = true;
        if ($again) {
            $again = false;
            $c = new C;
        }
        throw new Exception;
    }
}

$c = new C;

?>
--EXPECTF--
Fatal error: Uncaught Exception in %s:%d
Stack trace:
#0 [internal function]: C->__destruct()
#1 {main}
  thrown in %s on line %d
