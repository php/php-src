--TEST--
Test ksort() function: sorting escape sequences
--FILE--
<?php

const EXPECTED_RESULT = [
    "\t" => "\t",
    "\n" => "\n",
    "\v" => "\v",
    "\f" => "\f",
    "\r" => "\r",
    "\e" => "\e",
    "\a" => "\a",
    "\cx" => "\cx",
    "\ddd" => "\ddd",
    "\xhh" => "\xhh",
];

$array = [
    "\a" => "\a",
    "\cx" => "\cx",
    "\e" => "\e",
    "\f" => "\f",
    "\n" => "\n",
    "\r" => "\r",
    "\t" => "\t",
    "\xhh" => "\xhh",
    "\ddd" => "\ddd",
    "\v" => "\v",
];

echo "Default flag\n";
$temp_array = $array;
var_dump(ksort($temp_array)); // expecting : bool(true)
var_dump($temp_array === EXPECTED_RESULT);

echo "SORT_REGULAR\n";
$temp_array = $array;
var_dump(ksort($temp_array, SORT_REGULAR)); // expecting : bool(true)
var_dump($temp_array === EXPECTED_RESULT);

echo "SORT_STRING\n";
$temp_array = $array;
var_dump(ksort($temp_array, SORT_STRING)); // expecting : bool(true)
var_dump($temp_array === EXPECTED_RESULT);

?>
--EXPECT--
Default flag
bool(true)
bool(true)
SORT_REGULAR
bool(true)
bool(true)
SORT_STRING
bool(true)
bool(true)
