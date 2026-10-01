--TEST--
Test nullsafe strict type check
--FILE--
<?php

try {
    false?->bar();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    []?->bar();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    (0)?->bar();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    (0.0)?->bar();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    ''?->bar();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Call to a member function bar() on false
Error: Call to a member function bar() on array
Error: Call to a member function bar() on int
Error: Call to a member function bar() on float
Error: Call to a member function bar() on string
