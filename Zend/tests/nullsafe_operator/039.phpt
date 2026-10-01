--TEST--
Handling of undef variable exception in JMP_NULL
--FILE--
<?php

set_error_handler(function($_, $m) {
    throw new Exception($m);
});

try {
    $foo?->foo;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Exception: Undefined variable $foo
