--TEST--
errmsg: __construct() cannot be static
--FILE--
<?php

class test {

    static function __construct() {
    }
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Method test::__construct() cannot be static in %s on line %d
