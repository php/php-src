--TEST--
strlen() with a resource argument
--FILE--
<?php
//Test that passing a resource to strlen() throws a TypeError

$fp = fopen(__FILE__, 'r');
try {
    strlen($fp);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
fclose($fp);
?>
--EXPECTF--
strlen(): Argument #1 ($string) must be of type string, %s given
