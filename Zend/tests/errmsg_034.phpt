--TEST--
errmsg: __clone() cannot be static
--FILE--
<?php

class test {

    static function __clone() {
    }
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Method test::__clone() cannot be static in %s on line %d
