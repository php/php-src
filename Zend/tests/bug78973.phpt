--TEST--
Bug #78973: Destructor during CV freeing causes segfault if opline never saved
--INI--
opcache.optimization_level=0
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
#0  class@anonymous->__destruct() called at [%s:%d]
