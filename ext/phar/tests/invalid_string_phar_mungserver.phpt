--TEST--
Passing invalid string to Phar::mungServer()
--EXTENSIONS--
phar
--FILE--
<?php

$str = 'invalid';
try {
    Phar::mungServer([&$str]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

?>
--EXPECT--
ValueError: Phar::mungServer(): Argument #1 ($variables) must only contain elements with the following values "PHP_SELF", "REQUEST_URI", "SCRIPT_FILENAME", or "SCRIPT_NAME"
