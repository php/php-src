--TEST--
To string conversion failure in global
--FILE--
<?php

try {
    global ${new stdClass};
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Object of class stdClass could not be converted to string
