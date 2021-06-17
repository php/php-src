--TEST--
Bug #81160: isset/empty doesn't throw a TypeError on invalid string offset which is inconsistent compared to arrays
--FILE--
<?php

$s = 'Hello';
$o = new stdClass();
$a = [];
$r = STDERR;

echo "Objects as offsets:\n";
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
echo 'Array ($variable) as offsets:', "\n";
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
echo "Array (constant empty array) as offsets:\n";
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
echo "Resource as offsets:\n";
try {
    var_dump(isset($s[$r]));
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(empty($s[$r]));
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($s[$r] ?? 'default');
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Objects as offsets:

Warning: Cannot access offset of type stdClass in isset or empty in %s on line %d
bool(false)

Warning: Cannot access offset of type stdClass in isset or empty in %s on line %d
bool(true)
Cannot access offset of type stdClass in isset or empty
Array ($variable) as offsets:

Warning: Cannot access offset of type array in isset or empty in %s on line %d
bool(false)

Warning: Cannot access offset of type array in isset or empty in %s on line %d
bool(true)
Cannot access offset of type array in isset or empty
Array (constant empty array) as offsets:

Warning: Cannot access offset of type array in isset or empty in %s on line %d
bool(false)

Warning: Cannot access offset of type array in isset or empty in %s on line %d
bool(true)
Cannot access offset of type array in isset or empty
Resource as offsets:

Warning: Cannot access offset of type resource in isset or empty in %s on line %d
bool(false)

Warning: Cannot access offset of type resource in isset or empty in %s on line %d
bool(true)
Cannot access offset of type resource in isset or empty

