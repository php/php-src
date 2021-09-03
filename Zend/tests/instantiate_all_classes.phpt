--TEST--
Try to instantiate all classes without arguments
--FILE--
<?php

foreach (get_declared_classes() as $class) {
    try {
        new $class;
    } catch (Throwable) {}
}

?>
===DONE===
--EXPECT--
===DONE===
