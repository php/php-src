--TEST--
Destroy object in magic __get()
--FILE--
<?php
class Test {
    function __get($name) {
        $GLOBALS["x"] = null;
    }
}
$x = new Test;
var_dump($x->prop);

?>
--EXPECT--
NULL
