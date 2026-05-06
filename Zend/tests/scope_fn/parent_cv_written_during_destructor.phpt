--TEST--
A CV destructor that re-enters the parent's scope-fn body must not leak CVs auto-registered there
--FILE--
<?php
/* When a parent CV's destructor (here, the Fiber's destructor) runs scope-fn
 * body code that auto-registers a new CV in the parent (here, $e from the
 * catch), the new CV's slot may be one we've already iterated in
 * i_free_compiled_variables. The leave-time CV cleanup must handle that. */
function outer() {
    $fn = fn() {
        try {
            Fiber::suspend("paused");
        } finally {
            try {
                Fiber::suspend("from-finally");
            } catch (FiberError $e) {
                /* $e is auto-registered in outer() — the slot may already
                 * have been iterated by the time we get here. */
            }
        }
    };
    $fiber = new Fiber($fn);
    $fiber->start();
    /* outer() exits: $fiber CV release → fiber dtor → body finally → catch
     * writes $e. No leak should result. */
}
outer();
echo "ok\n";
?>
--EXPECT--
ok
