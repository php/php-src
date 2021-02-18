--TEST--
Spl\ForwardArrayIterator: protected from changes
--FILE--
<?php

$ht = [1];

$it = new Spl\ForwardArrayIterator($ht);

\array_pop($ht);
var_export($ht);
echo "\n";

echo 'Count of iterator: ', count($it), "\n";

foreach ($it as $key => $value) {
    var_export([$key => $value]);
    echo "\n";
}

?>
--EXPECTF--
array (
)
Count of iterator: 1
array (
  0 => 1,
)

