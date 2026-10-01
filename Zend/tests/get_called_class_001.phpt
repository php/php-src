--TEST--
Calling get_called_class() outside a class
--FILE--
<?php

try {
    var_dump(get_called_class());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: get_called_class() must be called from within a class
