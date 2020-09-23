--TEST--
foreach() & undefined var
--FILE--
<?php

try {
    foreach($a as $val);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

echo "Done\n";
?>
--EXPECTF--
Warning: Undefined variable $a in %s on line %d
foreach() argument must be of type array|object, null given
Done
