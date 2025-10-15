--TEST--
Passing invalid string to Phar::mungServer()
--FILE--
<?php

$str = 'invalid';
try {
    Phar::mungServer([&$str]);
} catch (PharException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Invalid value passed to Phar::mungServer(), expecting an array of any of these strings: PHP_SELF, REQUEST_URI, SCRIPT_FILENAME, SCRIPT_NAME
