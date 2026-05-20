--TEST--
Fiber captures a scope-fn closure but never starts: outer exit fires escape Error
--FILE--
<?php
$fiber = null;
function outer() {
    global $fiber;
    $fn = fn() { return 1; };
    $fiber = new Fiber($fn);
    /* never call $fiber->start() */
}

try {
    outer();
    echo "no error?\n";
} catch (Error $e) {
    echo "caught: ", $e->getMessage(), "\n";
}

/* Fiber is in INIT state; safe to drop. */
var_dump($fiber === null ? "null" : get_class($fiber));
$fiber = null;
echo "done\n";
?>
--EXPECT--
caught: Scope function closure must not outlive the declaring scope
string(5) "Fiber"
done
