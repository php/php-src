--TEST--
WeakMap dimension reference with a temporary key
--FILE--
<?php
$wm = new WeakMap;
try {
    $r = &$wm[new stdClass];
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump(count($wm));

try {
    $wm[new stdClass][] = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump(count($wm));

$key = new stdClass;
$wm[$key] = 2;
$keys = [$key];
$r = &$wm[array_pop($keys)];
$r = 3;
var_dump($wm[$key]);

$key = new stdClass;
$wm[$key] = 1;
$keys = [$key];
unset($key);
try {
    $r = &$wm[array_pop($keys)];
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump(count($wm));

class K {
    function __destruct() {
        $GLOBALS['wm'] = null;
    }
}

$wm = new WeakMap;
try {
    $wm[new K][] = str_repeat("x", 10);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($wm);
?>
--EXPECT--
Cannot modify a WeakMap dimension using a temporary object key
int(0)
Cannot modify a WeakMap dimension using a temporary object key
int(0)
int(3)
Cannot modify a WeakMap dimension using a temporary object key
int(0)
Cannot modify a WeakMap dimension using a temporary object key
int(0)
Cannot modify a WeakMap dimension using a temporary object key
NULL
