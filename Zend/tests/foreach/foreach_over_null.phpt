--TEST--
foreach over null
--FILE--
<?php

function test() {
    foreach (null as $v) {
        echo "Foo\n";
    }
}
test();

?>
--EXPECTF--
Warning: foreach() argument must be of type array|object, null given in %s on line %d
