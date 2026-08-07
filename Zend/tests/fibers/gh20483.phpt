--TEST--
GH-20483 (ASAN stack overflow with small fiber.stack_size INI value)
--INI--
fiber.stack_size=1024
--FILE--
<?php
$callback = function () {};
$fiber = new Fiber($callback);
try {
    $fiber->start();
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
Exception: Fiber stack size is too small, it needs to be at least %d bytes
