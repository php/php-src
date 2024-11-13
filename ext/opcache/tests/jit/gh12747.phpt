--TEST--
GH-12747: Function JIT returns invalid error message for coalesce operator on invalid offset
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php
$container = [];
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
Cannot access offset of type stdClass in isset or empty
empty():
Cannot access offset of type stdClass in isset or empty
Coalesce():
Cannot access offset of type stdClass on array
