--TEST--
BcMath\Number properties isset, empty, exists
--EXTENSIONS--
bcmath
--FILE--
<?php

$zero = new BcMath\Number(0);
$one = new BcMath\Number(1);
$has_frac = new BcMath\Number('5.67');

echo "========== isset ==========\n";
var_dump([
    'zero' => [
        'value' => isset($zero->value),
        'scale' => isset($zero->scale),
    ],
    'one' => [
        'value' => isset($one->value),
        'scale' => isset($one->scale),
    ],
    'has_frac' => [
        'value' => isset($has_frac->value),
        'scale' => isset($has_frac->scale),
    ],
]);
echo "\n";

echo "========== empty ==========\n";
var_dump([
    'zero' => [
        'value' => empty($zero->value),
        'scale' => empty($zero->scale),
    ],
    'one' => [
        'value' => empty($one->value),
        'scale' => empty($one->scale),
    ],
    'has_frac' => [
        'value' => empty($has_frac->value),
        'scale' => empty($has_frac->scale),
    ],
]);
echo "\n";

echo "========== property_exists ==========\n";
var_dump([
    'zero' => [
        'value' => property_exists($zero, 'value'),
        'scale' => property_exists($zero, 'scale'),
    ],
    'one' => [
        'value' => property_exists($one, 'value'),
        'scale' => property_exists($one, 'scale'),
    ],
    'has_frac' => [
        'value' => property_exists($has_frac, 'value'),
        'scale' => property_exists($has_frac, 'scale'),
    ],
]);
?>
--EXPECT--
========== isset ==========
array(3) {
  ["zero"]=>
  array(2) {
    ["value"]=>
    bool(true)
    ["scale"]=>
    bool(true)
  }
  ["one"]=>
  array(2) {
    ["value"]=>
    bool(true)
    ["scale"]=>
    bool(true)
  }
  ["has_frac"]=>
  array(2) {
    ["value"]=>
    bool(true)
    ["scale"]=>
    bool(true)
  }
}

========== empty ==========
array(3) {
  ["zero"]=>
  array(2) {
    ["value"]=>
    bool(true)
    ["scale"]=>
    bool(true)
  }
  ["one"]=>
  array(2) {
    ["value"]=>
    bool(false)
    ["scale"]=>
    bool(true)
  }
  ["has_frac"]=>
  array(2) {
    ["value"]=>
    bool(false)
    ["scale"]=>
    bool(false)
  }
}

========== property_exists ==========
array(3) {
  ["zero"]=>
  array(2) {
    ["value"]=>
    bool(true)
    ["scale"]=>
    bool(true)
  }
  ["one"]=>
  array(2) {
    ["value"]=>
    bool(true)
    ["scale"]=>
    bool(true)
  }
  ["has_frac"]=>
  array(2) {
    ["value"]=>
    bool(true)
    ["scale"]=>
    bool(true)
  }
}
