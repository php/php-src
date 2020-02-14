--TEST--
Cannot call user-defined functions from defaults of static variables
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
--EXPECTF--
Fatal error: Constant expression uses function log_call() which is not in get_const_expr_functions() in %s on line 8
