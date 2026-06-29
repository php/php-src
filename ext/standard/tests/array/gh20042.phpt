--TEST--
GH-20042 (SEGV in array.c when the error handler reassigns the array during prev())
--FILE--
<?php
set_error_handler(function ($errno, $errstr) {
    global $nan;
    $nan = bin2hex(random_bytes(4));
});
settype($nan, 'object');
try {
    prev($nan);
} catch (\Throwable $e) {
    echo get_class($e), "\n";
}
echo "OK survived\n";
?>
--EXPECT--
OK survived
