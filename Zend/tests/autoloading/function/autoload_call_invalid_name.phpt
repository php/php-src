--TEST--
Test autoload_call_function() with invalid symbol name
--FILE--
<?php
function customAutoloader($name) {
    var_dump($name);
}
autoload_register_function('customAutoloader');

try {
    autoload_call_function('12ayhs');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage();
}
try {
    autoload_call_function('"');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage();
}
try {
    autoload_call_function('');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage();
}
try {
    autoload_call_function("al\no");
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage();
}
?>
--EXPECT--
string(6) "12ayhs"
string(1) """
string(0) ""
string(4) "al
o"
