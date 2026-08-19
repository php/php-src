--TEST--
GH-9905 (constant() behaves inconsistent when class is undefined)
--FILE--
<?php
try {
    \constant("\NonExistantClass::non_existant_constant");
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Class "NonExistantClass" not found
