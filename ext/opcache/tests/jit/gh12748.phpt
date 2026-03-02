--TEST--
GH-12748: Function JIT emits "could not convert to int" warning at the same time as invalid offset Error
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php
$container = "string";
// Is
try {
    echo "isset():\n";
    var_dump(isset($container[new stdClass()]));
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    echo "empty():\n";
    var_dump(empty($container[new stdClass()]));
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    echo "Coalesce():\n";
    var_dump($container[new stdClass()] ?? 'default');
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
isset():
bool(false)
empty():
bool(true)
Coalesce():
Cannot access offset of type stdClass on string
