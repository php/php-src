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
        break;
    }
    echo $i;
}
echo PHP_EOL;
// new way break
for($i = 5; $i >= 0; $i--) {
    $i < 3 ? break : null;
    echo $i;
}
echo PHP_EOL;
for($i = 5; $i >= 0; $i--) {
    $i > 3 ?: break;
    echo $i;
}
echo PHP_EOL;
for($i = 5; $i >= 0; $i--) {
    $var = $i ?: break;
    echo $i;
}
?>
--EXPECTF--
543210
543
543
54
54321
