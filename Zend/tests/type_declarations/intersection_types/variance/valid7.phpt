--TEST--
Replacing object type with intersection type
--FILE--
<?php

interface X {}
interface Y {}

class Test {
    function method(): object {}
}
class Test2 extends Test {
    function method(): X&Y {}
}

?>
===DONE===
--EXPECTF--
TODO
