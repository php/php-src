--TEST--
Checking NAN in a switch statement with true/false
--FILE--
<?php

$nan = fdiv(0, 0);
switch ($nan) {
    case true:
        echo "true";
        break;
    case false:
        echo "false";
        break;
}
?>
--EXPECT--
true
