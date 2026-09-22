--TEST--
str_getcsv(): Invalid arguments
--FILE--
<?php

// string input[, string delimiter[, string enclosure[, string escape]]]
try {
    var_dump(str_getcsv('csv_string', 'separator', '"', ''));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(str_getcsv('csv_string', ',', 'enclosure', ''));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(str_getcsv('csv_string', ',', '"', 'escape'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: str_getcsv(): Argument #2 ($separator) must be a single character
ValueError: str_getcsv(): Argument #3 ($enclosure) must be a single character
ValueError: str_getcsv(): Argument #4 ($escape) must be empty or a single character
