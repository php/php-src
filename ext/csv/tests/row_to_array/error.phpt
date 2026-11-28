--TEST--
\ValueError conditions for Csv\row_to_array()
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

$string = "";

// Empty delimiter/enclosure/EOL sequence
try {
    var_dump(Csv\row_to_array($string, ''));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(Csv\row_to_array($string, ',', ''));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(Csv\row_to_array($string, ',', '"', ""));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

// Same delimiter and enclosure
try {
    var_dump(Csv\row_to_array($string, ',', ','));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

// Default value for the enclosure is "
try {
    var_dump(Csv\row_to_array($string, '"'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

// Delimiter or enclosure using the \r\n sequence
try {
    var_dump(Csv\row_to_array($string, "\r\n"));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

// Default value for the enclosure is "
try {
    var_dump(Csv\row_to_array($string, ',', "\r\n"));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
Csv\row_to_array(): Argument #2 ($delimiter) must not be empty
Csv\row_to_array(): Argument #3 ($enclosure) must not be empty
Csv\row_to_array(): Argument #4 ($eolSequence) must not be empty
Csv\row_to_array(): Argument #3 ($enclosure) must not be identical to argument #2 ($delimiter)
Csv\row_to_array(): Argument #3 ($enclosure) must not be identical to argument #2 ($delimiter)
Csv\row_to_array(): Argument #4 ($eolSequence) must not be identical to argument #2 ($delimiter)
Csv\row_to_array(): Argument #4 ($eolSequence) must not be identical to argument #3 ($enclosure)
