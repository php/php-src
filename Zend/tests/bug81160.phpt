--TEST--
Bug #81160: isset/empty doesn't throw a TypeError on invalid string offset
--FILE--
<?php

$s = 'Hello';
$o = new stdClass();
$a = [];

try {
    var_dump(isset($s[$o]));
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(empty($s[$o]));
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($s[$o] ?? 'default');
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(isset($s[$a]));
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(empty($s[$a]));
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($s[$a] ?? 'default');
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(isset($s[[]]));
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(empty($s[[]]));
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($s[[]] ?? 'default');
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot access offset of type stdClass on string
Cannot access offset of type stdClass on string
Cannot access offset of type stdClass on string
Cannot access offset of type array on string
Cannot access offset of type array on string
Cannot access offset of type array on string
Cannot access offset of type array on string
Cannot access offset of type array on string
Cannot access offset of type array on string
