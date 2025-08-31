--TEST--
#[\Override]: Declared abstract trait method.
--FILE--
<?php

trait T {
    public abstract function t(): void;
}

class C {
    use T;

    #[\Override]
    public function t(): void {}
}

echo "Done";

?>
--EXPECT--
Done
