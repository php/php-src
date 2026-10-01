--TEST--
Bug #44660 (Indexed and reference assignment to property of non-object don't trigger warning)
--FILE--
<?php
$s = "hello";
$a = true;

echo "--> read access:";
echo $a->p;

echo "\n--> direct assignment:\n";
try {
    $a->p = $s;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "\n--> increment:\n";
try {
    $a->p++;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "\n--> reference assignment:\n";
try {
    $a->p =& $s;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "\n--> reference assignment:\n";
try {
    $s =& $a->p;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "\n--> indexed assignment:\n";
try {
    $a->p[0] = $s;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "\n--> Confirm assignments have had no impact:\n";
var_dump($a);
?>
--EXPECTF--
--> read access:
Warning: Attempt to read property "p" on true in %s on line %d

--> direct assignment:
Error: Attempt to assign property "p" on true

--> increment:
Error: Attempt to increment/decrement property "p" on true

--> reference assignment:
Error: Attempt to modify property "p" on true

--> reference assignment:
Error: Attempt to modify property "p" on true

--> indexed assignment:
Error: Attempt to modify property "p" on true

--> Confirm assignments have had no impact:
bool(true)
