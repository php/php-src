--TEST--
mb_ereg() compatibility test 2 (counterpart: ext/standard/tests/reg/005.phpt)
--POST--
--GET--
--FILE--
<?php $a="This is a nice and simple string";
  echo mb_ereg(".*(is).*(is).*",$a,$registers);
  echo "\n";
  echo $registers[0];
  echo "\n";
  echo $registers[1];
  echo "\n";
  echo $registers[2];
  echo "\n";
?>
--EXPECT--
32
This is a nice and simple string
is
is
