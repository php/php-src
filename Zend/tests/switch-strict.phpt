--TEST--
Strict switch cases
--FILE--
<?php

$a = 123;

switch ($a) {
  case === '123':
    echo "Should not match\n";
    break;
  case == '123':
    echo "Should match\n";
    break;
  default:
    echo "Default\n";
}

switch ($a) {
  case === 123.0:
    echo "Should not match\n";
    break;
  case 123.0:
    echo "Should match\n";
    break;
  default:
    echo "Default\n";
}
--EXPECT--
Should match
Should match
