--TEST--
Context manager: exception is forwarded to manager
--FILE--
<?php

function some_context() {
    try {
        yield 'hello';
    } catch (Exception $e) {
        echo "From context manager: ", $e::class, ": ", $e->getMessage(), "\n";
    }
}

using (some_context() => $var) {
    throw new Exception('Thrown from body');
};

echo "\$var is unset() after the block: ";
var_dump(!isset($var));

?>
--EXPECT--
From context manager: Exception: Thrown from body
$var is unset() after the block: bool(true)
