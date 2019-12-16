--TEST--
Bug #78973: Destructor during CV freeing causes segfault if opline never saved
--FILE--
<?php

function test($x) {
}
test(new class {
    public function __destruct() {
        debug_print_backtrace();
    }
});

?>
--EXPECTF--
#0  class@anonymous->__destruct() called at [%s:4]
#1  test() called at [%s:5]
