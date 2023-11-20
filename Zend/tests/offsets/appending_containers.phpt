--TEST--
Appending containers
--FILE--
<?php

$containers = [
    null,
    false,
    true,
    10,
    25.5,
    'string',
    [],
    new stdClass(),
    STDERR,
];

foreach ($containers as $container) {
    try {
        $container[] = 'value';
        var_dump($container);
    } catch (\Throwable $e) {
        echo $e->getMessage(), "\n";
    }
}

?>
--EXPECTF--
array(1) {
  [0]=>
  string(5) "value"
}

Deprecated: Automatic conversion of false to array is deprecated in %s on line %d
array(1) {
  [0]=>
  string(5) "value"
}
Cannot use a scalar value as an array
Cannot use a scalar value as an array
Cannot use a scalar value as an array
[] operator not supported for strings
array(1) {
  [0]=>
  string(5) "value"
}
Cannot use object of type stdClass as array
Cannot use a scalar value as an array
