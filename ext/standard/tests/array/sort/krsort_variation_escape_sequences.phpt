--TEST--
Test krsort() function: sorting escape sequences
--FILE--
<?php

const EXPECTED_RESULT = [
    "\xhh" => "\xhh",
    "\ddd" => "\ddd",
    "\cx" => "\cx",
    "\a" => "\a",
    "\e" => "\e",
    "\r" => "\r",
    "\f" => "\f",
    "\v" => "\v",
    "\n" => "\n",
    "\t" => "\t",
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
var_dump(krsort($temp_array)); // expecting : bool(true)
var_dump($temp_array === EXPECTED_RESULT);

echo "SORT_REGULAR\n";
$temp_array = $array;
var_dump(krsort($temp_array, SORT_REGULAR)); // expecting : bool(true)
var_dump($temp_array === EXPECTED_RESULT);

echo "SORT_STRING\n";
$temp_array = $array;
var_dump(krsort($temp_array, SORT_STRING)); // expecting : bool(true)
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
