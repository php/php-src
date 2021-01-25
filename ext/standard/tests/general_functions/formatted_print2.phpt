--TEST--
println() function with strict_types=1 throws for non-string
--FILE--
<?php

declare(strict_types=1);

$v1 = println('println always adds a newline');
println("wrote $v1 bytes");
try {
    println(1);
} catch (TypeError $e) {
    println("Caught: " . $e->getMessage());
}
println();  // valid
try {
    println(null);
} catch (TypeError $e) {
    println("Caught: " . $e->getMessage());
}

?>
--EXPECT--
println always adds a newline
wrote 30 bytes
Caught: println(): Argument #1 ($data) must be of type string, int given

Caught: println(): Argument #1 ($data) must be of type string, null given
