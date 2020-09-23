--TEST--
foreach over null
--FILE--
<?php

function test() {
    try {
        foreach (null as $v) {
            echo "Foo\n";
        }
    } catch (\TypeError $e) {
        echo $e->getMessage(), \PHP_EOL;
    }
}
test();

?>
--EXPECT--
foreach() argument must be of type array|object, null given
