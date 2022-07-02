--TEST--
Calling get_called_class() outside a class
--FILE--
<?php

try {
    var_dump(get_called_class());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
get_called_class() must be called from within a class
