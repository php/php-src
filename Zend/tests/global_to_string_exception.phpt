--TEST--
To string conversion failure in global
--FILE--
<?php

try {
    global ${new stdClass};
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Object of class stdClass could not be converted to string
