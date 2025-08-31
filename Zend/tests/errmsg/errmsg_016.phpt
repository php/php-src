--TEST--
errmsg: __isset() must take exactly 1 argument
--FILE--
<?php

class test {
    function __isset() {
    }
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Method test::__isset() must take exactly 1 argument in %s on line %d
