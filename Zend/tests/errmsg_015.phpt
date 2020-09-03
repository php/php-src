--TEST--
errmsg: __clone() cannot take more than 1 argument
--FILE--
<?php

class test {
    function __clone($origThis, $var) {
    }
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Method test::__clone() cannot take more than 1 argument in %s on line %d
