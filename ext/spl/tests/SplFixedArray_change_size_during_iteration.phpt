--TEST--
SPL: FixedArray: change array size during iteration
--FILE--
<?php

$splFixedArray = SplFixedArray::fromArray(["a","b","c"]);
/* Try changing size on first, second, and final iterations, and check what
 * happens in each case */
foreach ($splFixedArray as $k => $v) {
  echo "$k => $v\n";
  if ($k == 0) {
    $splFixedArray->setSize(2);
  }
}
echo "---\n";

$splFixedArray = SplFixedArray::fromArray(["a","b","c"]);
foreach ($splFixedArray as $k => $v) {
  echo "$k => $v\n";
  if ($k == 1) {
    $splFixedArray->setSize(2);
  }
}
echo "---\n";

$splFixedArray = SplFixedArray::fromArray(["a","b","c"]);
foreach ($splFixedArray as $k => $v) {
  echo "$k => $v\n";
  if ($k == 2) {
    $splFixedArray->setSize(2);
  }
}
echo "\n";
--EXPECT--
0 => a
1 => b
---
0 => a
1 => b
---
0 => a
1 => b
2 => c
