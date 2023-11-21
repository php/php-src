--TEST--
Invalid array offset types throws TypeError on isset/empty/coalese
--FILE--
<?php

$v = ['Hello', 'world'];
$o = new stdClass();
$a = [];
$r = STDERR;

echo "Objects as offsets:\n";
try {
    var_dump(isset($v[$o]));
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(empty($v[$o]));
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($v[$o] ?? 'default');
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
echo 'Array ($variable) as offsets:', "\n";
try {
    var_dump(isset($v[$a]));
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(empty($v[$a]));
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($v[$a] ?? 'default');
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
echo "Array (constant empty array) as offsets:\n";
try {
    var_dump(isset($v[[]]));
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(empty($v[[]]));
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($v[[]] ?? 'default');
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
echo "Resource as offsets:\n";
try {
    var_dump(isset($v[$r]));
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(empty($v[$r]));
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($v[$r] ?? 'default');
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Objects as offsets:
Cannot access offset of type stdClass in isset or empty
Cannot access offset of type stdClass in isset or empty
Cannot access offset of type stdClass on array
Array ($variable) as offsets:
Cannot access offset of type array in isset or empty
Cannot access offset of type array in isset or empty
Cannot access offset of type array on array
Array (constant empty array) as offsets:
Cannot access offset of type array in isset or empty
Cannot access offset of type array in isset or empty
Cannot access offset of type array on array
Resource as offsets:

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line %d
bool(false)

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line %d
bool(true)

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line %d
string(7) "default"

