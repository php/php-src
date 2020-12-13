--TEST--
Test multiple default arms in match in different arms
--FILE--
<?php

$a = 3;

match {
  $a < 2 => 'small',
  $a == 3 => 'medium',
  default => 'large',
};

?>
--EXPECTF--
medium
