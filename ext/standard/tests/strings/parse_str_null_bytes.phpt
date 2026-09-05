--TEST--
parse_str() rejects null bytes
--FILE--
<?php

try {
    parse_str("a=1\0&b=2", $result);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: parse_str(): Argument #1 ($string) must not contain any null bytes
