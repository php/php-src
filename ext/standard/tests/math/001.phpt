--TEST--
Simple math tests
--POST--
--GET--
--FILE--
<?php 
  echo abs(-1) . "\n";
  echo abs(-1.5) . "\n";
  echo abs("-1") . "\n";
  echo abs("-1.5") . "\n";
  echo ceil(-1.5) . "\n";
  echo ceil(1.5) . "\n";
  echo floor(-1.5) . "\n";
  echo floor(1.5) . "\n";
?>
--EXPECT--

1
1.5
1
1.5
-1
2
-2
1
