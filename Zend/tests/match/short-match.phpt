--TEST--
Short-hand match with implicit true subject.
--FILE--
<?php

$a = 3;

print match {
  $a < 2 => 'small',
  $a == 3 => 'medium',
  default => 'large',
};

?>
--EXPECTF--
medium
