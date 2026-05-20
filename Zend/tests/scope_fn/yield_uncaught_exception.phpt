--TEST--
Uncaught exception inside scope-fn generator body propagates to caller of next()
--FILE--
<?php
function outer() {
    $fn = fn() {
        yield 1;
        throw new RuntimeException("oops");
    };
    $g = $fn();
    var_dump($g->current()); // 1
    try {
        $g->next();
    } catch (RuntimeException $e) {
        echo "caught: ", $e->getMessage(), "\n";
    }
    /* Generator is finished after the throw. */
    var_dump($g->valid());
}
outer();
echo "done\n";
?>
--EXPECT--
int(1)
caught: oops
bool(false)
done
