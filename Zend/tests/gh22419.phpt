--TEST--
GH-22419 (UAF/refcount assertion in array-offset concat when the error handler replaces the LHS)
--FILE--
<?php
for ($i = 0; $i < 2; $i++) {
    $a = [[1], [2]];
    set_error_handler(function() use (&$a) {
        $a = "changed";
    });
    try {
        $a[1] .= 1234;
    } catch (Throwable $e) {
    }
    restore_error_handler();
}
echo "ok\n";
?>
--EXPECT--
ok
