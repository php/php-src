--TEST--
Bad unserialize_callback_func
--FILE--
<?php
ini_set('unserialize_callback_func','Nonexistent');
try {
    unserialize('O:3:"FOO":0:{}');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Invalid callback Nonexistent, function "Nonexistent" not found or invalid function name
