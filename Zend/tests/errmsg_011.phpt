--TEST--
errmsg: cannot redeclare (method)
--FILE--
<?php

class test {

    function foo() {}
    function foo() {}

}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Method test::foo() cannot be redeclared in %s on line %d
