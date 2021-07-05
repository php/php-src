--TEST--
Replacing object type with not-loadable intersection type
--FILE--
<?php


class Test {
    function method(): object {}
}
class Test2 extends Test {
    function method(): X&Y {}
}

?>
===DONE===
--EXPECT--
===DONE===
