--TEST--
#[\Override]: On used trait with interface method.
--FILE--
<?php

trait T {
    #[\Override]
    public function i(): void {}
}

interface I {
    public function i(): void;
}

class Foo implements I {
    use T;
}

echo "Done";

?>
--EXPECT--
Done
