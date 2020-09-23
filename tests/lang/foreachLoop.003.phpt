--TEST--
Foreach loop tests - error case: not an array.
--FILE--
<?php
echo "\nNot an array.\n";
$a = TRUE;
try {
    foreach ($a as $v) {
        var_dump($v);
    }
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

$a = null;
try {
    foreach ($a as $v) {
        var_dump($v);
    }
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

$a = 1;
try {
    foreach ($a as $v) {
        var_dump($v);
    }
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

$a = 1.5;
try {
    foreach ($a as $v) {
        var_dump($v);
    }
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

$a = "hello";
try {
    foreach ($a as $v) {
        var_dump($v);
    }
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

echo "done.\n";
?>
--EXPECTF--
Not an array.
foreach() argument must be of type array|object, bool given
foreach() argument must be of type array|object, null given
foreach() argument must be of type array|object, int given
foreach() argument must be of type array|object, float given
foreach() argument must be of type array|object, string given
done.
