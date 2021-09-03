--TEST--
mb_ereg() compat test 2
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_ereg') or die("skip mb_ereg() is not available in this build");
?>
--FILE--
<?php
/* (counterpart: ext/standard/tests/reg/005.phpt) */
  $a="This is a nice and simple string";
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
1
This is a nice and simple string
is
is
