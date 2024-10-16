--TEST--
Try to instantiate all classes without arguments
--EXTENSIONS--
*
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
