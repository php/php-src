--TEST--
Early break
--FILE--
<?php
// no break
for($i = 5; $i >= 0; $i--) {
  echo $i;
}
echo PHP_EOL;
// old way break
for($i = 5; $i >= 0; $i--) {
    if ($i < 3) {
        continue;
    }
    echo $i;
}

// new way continue
echo PHP_EOL;
for($i = 5; $i >= 0; $i--) {
    $i > 3 ?: continue;
    echo $i;
}
echo PHP_EOL;
for($i = 5; $i >= 0; $i--) {
    $var = $i ?: continue;
    echo $i;
}
?>
--EXPECT--
543210
543
54
54321
