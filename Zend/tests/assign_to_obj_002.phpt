--TEST--
Assign to $this leaks when $this not defined
--FILE--
<?php

try {
    $this->a = new stdClass;
} catch (Error $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

?>
--EXPECT--
Error: Using $this when not in object context
