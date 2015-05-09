--TEST--
Test array_walk() with callback using same subject array by reference
--FILE--
<?php

$parts = array (
  'foo',
  'bar',
  'baz',
);

array_walk($parts, function ($value, $key) use (&$parts) {
    $parts = array_values($parts);
});

echo "Done\n";
?>
--EXPECTF--
Done
