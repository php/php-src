--TEST--
Can call user-defined functions from defaults of static variables
--FILE--
<?php
function log_call(string $arg) {
    echo "log_call('$arg')\n";
    return $arg;
}

$f = function () {
    static $call = log_call(sprintf("Hello, %s", "World"));
    echo "$call\n";
};
$f();
$f();
?>
--EXPECT--
log_call('Hello, World')
Hello, World
Hello, World
