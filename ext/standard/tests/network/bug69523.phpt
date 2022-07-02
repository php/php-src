--TEST--
setcookie() allows empty cookie name
--FILE--
<?php
try {
    setcookie('', 'foo');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
setcookie(): Argument #1 ($name) cannot be empty
