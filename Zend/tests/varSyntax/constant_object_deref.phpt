--TEST--
Constants can be dereferenced as objects (even though they can't be objects)
--FILE--
<?php

const FOO = "foo";
class Bar { const FOO = "foo"; }

try {
    FOO->length();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    Bar::FOO->length();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Call to a member function length() on string
Error: Call to a member function length() on string
