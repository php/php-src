--TEST--
Nullable covariant return types
--FILE--
<?php

interface A {
    function method(): ?int;
}

interface B extends A {
    function method(): int;
}
?>
--EXPECT--
