--TEST--
Request #15184 (Calculate the quarter with date('Q')?)
--FILE--
<?php
for($m = 1 ; $m <= 12 ; $m++)
  echo date('q', mktime(0, 0, 0, $m, 1, 2000));

echo "\nDONE\n";
?>
--EXPECT--
111222333444
DONE
