--TEST--
Testing Closure self-reference functionality: This binding
--FILE--
<?php

$closure = null;
$fn = function(bool $root = false, bool $set = false) as $fn use(&$closure) : void {
    echo $this::A, PHP_EOL;
    if ($set) {
        $closure = $fn;
    }
    if ($root) {
        $fn();
    }
    return;
};
$fn = $fn->bindTo(new class{const A="bind";});
$fn(true, true);
$fn->call(new class{const A="call";}, true, true);
$fn(true, false);
$closure(true, false);

?>
--EXPECTF--
bind
bind
call
call
bind
bind
call
call
