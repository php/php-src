--TEST--
Test rsort() function: sorting escape sequences
--FILE--
<?php

const EXPECTED_RESULT = [
    "\xhh",
    "\ddd",
    "\cx",
    "\a",
    "\e",
    "\r",
    "\f",
    "\v",
    "\n",
    "\t",
    null,
];

$array = [
    null,
    "\a",
    "\cx",
    "\e",
    "\f",
    "\n",
    "\r",
    "\t",
    "\xhh",
    "\ddd",
    "\v",
];

echo "Default flag\n";
$temp_array = $array;
var_dump(rsort($temp_array)); // expecting : bool(true)
var_dump($temp_array === EXPECTED_RESULT);

echo "SORT_REGULAR\n";
$temp_array = $array;
var_dump(rsort($temp_array, SORT_REGULAR)); // expecting : bool(true)
var_dump($temp_array === EXPECTED_RESULT);

echo "SORT_STRING\n";
$temp_array = $array;
var_dump(rsort($temp_array, SORT_STRING)); // expecting : bool(true)
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
