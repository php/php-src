--TEST--
Dynamic autoload with invalid symbol name in variable
--FILE--
<?php
function customAutoloader($name) {
    var_dump($name);
}
autoload_register_function('customAutoloader');
$name = '12ayhs';

try {
    $name();
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
$name = '"';
try {
    $name();
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
$name = '';
try {
    $name();
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
$name = "al\no";
try {
    $name();
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
?>
--EXPECT--
string(6) "12ayhs"
Error: Call to undefined function 12ayhs()
Error: Call to undefined function "()
Error: Call to undefined function ()
Error: Call to undefined function al
o()
