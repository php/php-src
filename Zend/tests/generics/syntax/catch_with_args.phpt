--TEST--
Generic syntax: catch with type arguments (args discarded at runtime)
--FILE--
<?php
class MyErr extends Exception {}
try {
    throw new MyErr('boom');
} catch (MyErr<int> $e) {
    echo "caught: ", $e->getMessage(), "\n";
}
?>
--EXPECT--
caught: boom
