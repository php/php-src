--TEST--
Bug #72162 (use-after-free - error_reporting)
--FILE--
<?php
error_reporting(E_ALL);
$var11 = new StdClass();

try {
    $var16 = error_reporting($var11);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
error_reporting(): Argument #1 ($error_level) must be of type ?int, stdClass given
