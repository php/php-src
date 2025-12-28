--TEST--
strlen() with an object without __toString()
--FILE--
<?php

try {
    strlen(new stdClass());
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
strlen(): Argument #1 ($string) must be of type string, %s given
