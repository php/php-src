--TEST--
GH-9905 (constant() behaves inconsistent when class is undefined)
--FILE--
<?php
try {
    \constant("\NonExistantClass::non_existant_constant");
} catch (\Throwable|\Error|\Exception $e) {
    echo($e->getMessage());
}
?>
--EXPECT--
Class "NonExistantClass" not found
