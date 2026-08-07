--TEST--
setcookie() allows empty cookie name
--FILE--
<?php
try {
    setcookie('', 'foo');
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
ValueError: setcookie(): Argument #1 ($name) must not be empty
