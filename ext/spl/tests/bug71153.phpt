--TEST--
Bug #71153: Performance Degradation in ArrayIterator with large arrays
--FILE--
<?php

$n = 200000;

for ($i = 0; $i < $n; ++$i) {
    foreach (new ArrayIterator([]) as $v) {}
}

echo "done\n";

?>
--EXPECT--
done
