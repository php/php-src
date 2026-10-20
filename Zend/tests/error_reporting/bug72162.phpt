--TEST--
Bug #72162 (use-after-free - error_reporting)
--FILE--
<?php
error_reporting(E_ALL);
$var11 = new StdClass();

try {
    $var16 = error_reporting($var11);
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: error_reporting(): Argument #1 ($error_level) must be of type ?int, stdClass given
