--TEST--
Pass short ctor parameters
--FILE--
<?php

try {
    assert(false && function () {
    class Empty0 ();
    });
} catch (Throwable $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
assert(false && function () {
    class Empty0 {
        public function __construct() {
        }

    }

})