--TEST--
list() with undefined keys
--FILE--
<?php

$contrivedMixedKeyTypesExample = [
    7 => "the best PHP version",
    "elePHPant" => "the cutest mascot"
];

list(5 => $five, "duke" => $duke) = $contrivedMixedKeyTypesExample;

var_dump($five, $duke);

?>
--EXPECTF--
Warning: Undefined array key 5 in %s on line %d

Warning: Undefined array key "duke" in %s on line %d
NULL
NULL
