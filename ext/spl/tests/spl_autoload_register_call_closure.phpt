--TEST--
spl_autoload_register() refuses a Closure of spl_autoload_call()
--FILE--
<?php
$closures = [
    spl_autoload_call(...),
    Closure::fromCallable('spl_autoload_call'),
];

foreach ($closures as $closure) {
    try {
        spl_autoload_register($closure);
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}

spl_autoload_register(strlen(...));
var_dump(count(spl_autoload_functions()));
var_dump(class_exists('Missing'));
?>
--EXPECT--
spl_autoload_register(): Argument #1 ($callback) must not be the spl_autoload_call() function
spl_autoload_register(): Argument #1 ($callback) must not be the spl_autoload_call() function
int(1)
bool(false)
