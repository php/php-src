--TEST--
Trying to parse a non existent file
--EXTENSIONS--
tidy
--FILE--
<?php

$tidy = new tidy;

try {
    $tidy->parseFile("does_not_exist.html");
} catch (Throwable $e) {
    echo $e::class . '::' . $e->getMessage(), PHP_EOL;
}

try {
    tidy_parse_file("does_not_exist.html");
} catch (Throwable $e) {
    echo $e::class . '::' . $e->getMessage(), PHP_EOL;
}

try {
    $tidy = new tidy("does_not_exist.html");
} catch (Throwable $e) {
    echo $e::class . '::' . $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
TidyException::Cannot load "does_not_exist.html" into memory
TidyException::Cannot load "does_not_exist.html" into memory
TidyException::Cannot load "does_not_exist.html" into memory
