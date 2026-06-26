--TEST--
ENTER_SCOPE_FUNC arg→CV remap is exception-safe when a parent CV's destructor throws
--FILE--
<?php
class Boom {
    public function __destruct() {
        throw new Error('dtor boom');
    }
}

function go() {
    /* $x is the parent CV that the closure parameter `$x` will overwrite. */
    $x = new Boom;
    $f = fn($x) {
        return 'body got: ' . $x;
    };
    try {
        /* Closure call: arg 42 is moved into parent CV $x.
         * Old value of $x (Boom) gets destructed locally — its
         * __destruct throws AFTER dst already holds the new value (42),
         * so dst is not corrupted; the exception propagates out. */
        $r = $f(42);
        var_dump($r);
    } catch (Error $e) {
        echo "caught: ", $e->getMessage(), "\n";
    }
    /* dst was set to 42 BEFORE old.dtor ran. So $x is now 42, even though
     * the dtor threw. */
    var_dump($x);
}
go();
--EXPECTF--
caught: dtor boom
int(42)
