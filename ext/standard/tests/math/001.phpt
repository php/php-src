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
  echo abs(-2147483647) . "\n";
  echo abs(-2147483648) . "\n";
  echo abs(-2147483649) . "\n";
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
2147483647
2147483648
2147483649
-1
2
-2
1
