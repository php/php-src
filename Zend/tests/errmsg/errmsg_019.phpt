--TEST--
errmsg: __destruct() cannot take arguments
--FILE--
<?php

class test {
    function __destruct($var) {
    }
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Method test::__destruct() cannot take arguments in %s on line %d
