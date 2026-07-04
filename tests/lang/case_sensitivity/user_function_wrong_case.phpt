--TEST--
User-defined function called with wrong case fails
--FILE--
<?php
function myUserFunction() {
    return "user func result";
}
try {
    echo MYUSERFUNCTION() . "\n";
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
echo myUserFunction() . "\n";
?>
--EXPECT--
Call to undefined function MYUSERFUNCTION()
user func result
