--TEST--
parse_ini_file and parse_ini_string throw ValueError exceptions on invalid $scanner_mode param values
--FILE--
<?php

try {
    parse_ini_file('x', scanner_mode: 42);
} catch (Throwable $e) {
    echo $e::class . ': '.$e->getMessage(), "\n";
}

try {
    parse_ini_string('x', scanner_mode: 42);
} catch (Throwable $e) {
    echo $e::class . ': '.$e->getMessage(), "\n";
}

echo "Done"
?>
--EXPECT--
ValueError: parse_ini_file(): Argument #3 ($scanner_mode) must be one of INI_SCANNER_NORMAL, INI_SCANNER_RAW, or INI_SCANNER_TYPED
ValueError: parse_ini_string(): Argument #3 ($scanner_mode) must be one of INI_SCANNER_NORMAL, INI_SCANNER_RAW, or INI_SCANNER_TYPED
Done
