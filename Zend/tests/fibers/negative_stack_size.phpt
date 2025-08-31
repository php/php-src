--TEST--
fiber.stack_size must be a positive number
--FILE--
<?php
ini_set("fiber.stack_size","-1");
$fiber = new Fiber(function() {});
try {
    $fiber->start();
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage()."\n";
}
?>
DONE
--EXPECTF--
Warning: fiber.stack_size must be a positive number in %snegative_stack_size.php on line 2
DONE
