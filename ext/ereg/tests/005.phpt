--TEST--
Test Regular expression register support in ereg
--SKIPIF--
<?php if (unicode_semantics()) die('skip unicode.semantics=on'); ?>        
--FILE--
<?php $a="This is a nice and simple string";
  echo ereg(".*(is).*(is).*",$a,$registers);
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
