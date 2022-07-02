--TEST--
Constant propagation with nullsafe operator
--FILE--
<?php

class Bar { const FOO = "foo"; }

try {
    Bar::FOO?->length();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Call to a member function length() on string
