--TEST--
Instantiate UuidV7 from an UUID v4 string
--FILE--
<?php

try {
    new Uuid\UuidV7("d70f042a-c5ca-4726-b259-795e47fd1b95");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Exception: The specified UUID v7 is malformed
