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
Notice: Undefined array offset 5 in %s on line %d

Notice: Undefined array index "duke" in %s on line %d
NULL
NULL
