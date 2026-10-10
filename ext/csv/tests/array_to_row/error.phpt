--TEST--
\ValueError conditions for Csv\array_to_row()
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

$fields = [];

// Empty delimiter/enclosure/EOL sequence
try {
    var_dump(Csv\array_to_row($fields, ''));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(Csv\array_to_row($fields, ',', ''));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(Csv\array_to_row($fields, ',', '"', ""));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

// Same delimiter and enclosure
try {
    var_dump(Csv\array_to_row($fields, ',', ','));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

// Default value for the enclosure is "
try {
    var_dump(Csv\array_to_row($fields, '"'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

// Delimiter or enclosure using the \r\n sequence
try {
    var_dump(Csv\array_to_row($fields, "\r\n"));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

// Default value for the enclosure is "
try {
    var_dump(Csv\array_to_row($fields, ',', "\r\n"));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
Csv\array_to_row(): Argument #2 ($delimiter) must not be empty
Csv\array_to_row(): Argument #3 ($enclosure) must not be empty
Csv\array_to_row(): Argument #4 ($eolSequence) must not be empty
Csv\array_to_row(): Argument #3 ($enclosure) must not be identical to argument #2 ($delimiter)
Csv\array_to_row(): Argument #3 ($enclosure) must not be identical to argument #2 ($delimiter)
Csv\array_to_row(): Argument #4 ($eolSequence) must not be identical to argument #2 ($delimiter)
Csv\array_to_row(): Argument #4 ($eolSequence) must not be identical to argument #3 ($enclosure)
