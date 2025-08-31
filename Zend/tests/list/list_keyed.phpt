--TEST--
list() with keys
--FILE--
<?php

$antonyms = [
    "good" => "bad",
    "happy" => "sad",
];

list("good" => $good_antonym, "happy" => $happy_antonym) = $antonyms;
var_dump($good_antonym, $happy_antonym);

echo PHP_EOL;

$powersOfTwo = [
    1 => 2,
    2 => 4,
    3 => 8
];

list(1 => $two_1, 2 => $two_2, 3 => $two_3) = $powersOfTwo;
var_dump($two_1, $two_2, $two_3);

echo PHP_EOL;

$contrivedMixedKeyTypesExample = [
    7 => "the best PHP version",
    "elePHPant" => "the cutest mascot"
];

list(7 => $seven, "elePHPant" => $elePHPant) = $contrivedMixedKeyTypesExample;
var_dump($seven, $elePHPant);

echo PHP_EOL;

$allTogetherNow = [
    "antonyms" => $antonyms,
    "powersOfTwo" => $powersOfTwo,
    "contrivedMixedKeyTypesExample" => $contrivedMixedKeyTypesExample
];

list(
    "antonyms" => list("good" => $good_antonym, "happy" => $happy_antonym),
    "powersOfTwo" => list(1 => $two_1, 2 => $two_2, 3 => $two_3),
    "contrivedMixedKeyTypesExample" => list(7 => $seven, "elePHPant" => $elePHPant)
) = $allTogetherNow;

var_dump($good_antonym, $happy_antonym);
var_dump($two_1, $two_2, $two_3);
var_dump($seven, $elePHPant);

?>
--EXPECT--
string(3) "bad"
string(3) "sad"

int(2)
int(4)
int(8)

string(20) "the best PHP version"
string(17) "the cutest mascot"

string(3) "bad"
string(3) "sad"
int(2)
int(4)
int(8)
string(20) "the best PHP version"
string(17) "the cutest mascot"
