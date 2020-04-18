--TEST--
Constants can be dereferenced as objects (even though they can't be objects)
--FILE--
<?php

const FOO = "foo";
class Bar { const FOO = "foo"; }

try {
    FOO->length();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    Bar::FOO->length();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Call to a member function length() on string
Call to a member function length() on string
