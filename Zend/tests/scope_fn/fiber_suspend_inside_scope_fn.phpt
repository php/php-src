--TEST--
Fiber::suspend called directly from within a scope-fn body (non-generator), then parent exits
--FILE--
<?php
$fiber = null;
function outer() {
    global $fiber;
    $fn = fn() {
        Fiber::suspend("inside-fn");
    };

    $fiber = new Fiber(function () use ($fn) {
        try {
            $fn();
            echo "after fn (unreachable)\n";
        } catch (Throwable $e) {
            /* The throw materializes here, at the suspension resumption
             * point inside the fiber. The stacktrace shows the Error
             * originated from within the scope-fn body. */
            echo $e, "\n";
        }
    });
    var_dump($fiber->start()); // string(9) "inside-fn"
}

try {
    outer();
} catch (Error $e) {
    echo "outer: ", $e->getMessage(), "\n";
}

var_dump($fiber->resume()); // null — fiber body finished after catch

$fiber = null;
echo "done\n";
?>
--EXPECTF--
string(9) "inside-fn"
outer: Scope function closure must not outlive the declaring scope
Error: Scope function closure must not outlive the declaring scope in %sfiber_suspend_inside_scope_fn.php:%d
Stack trace:
#0 %sfiber_suspend_inside_scope_fn.php(%d): {closure:%s}()
#1 [internal function]: {closure:%s}()
#2 %sfiber_suspend_inside_scope_fn.php(%d): unknown()
#3 {main}
NULL
done
