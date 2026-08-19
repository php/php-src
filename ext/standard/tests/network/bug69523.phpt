--TEST--
setcookie() allows empty cookie name
--FILE--
<?php
try {
    setcookie('', 'foo');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: setcookie(): Argument #1 ($name) must not be empty
