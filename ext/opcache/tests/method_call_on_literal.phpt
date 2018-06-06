--TEST--
Literal compaction should take method calls on literals into account
--FILE--
<?php

try {
    (42)->foo();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Call to a member function foo() on int
