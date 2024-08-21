--TEST--
Dynamic autoload with invalid symbol name in variable
--FILE--
<?php
function customAutoloader($name) {
    var_dump($name);
}
autoload_register_class('customAutoloader');
$name = '12ayhs';

try {
    new $name;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
$name = '"';
try {
    new $name;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
$name = '';
try {
    new $name;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
$name = "al\no";
try {
    new $name;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
?>
--EXPECT--
string(6) "12ayhs"
Error: Class "12ayhs" not found
Error: Class """ not found
Error: Class "" not found
Error: Class "al
o" not found
