--TEST--
#[\Override]: Valid anonymous class.
--FILE--
<?php

interface I {
    public function i(): void;
}

new class () implements I {
    #[\Override]
    public function i(): void {}
};

echo "Done";

?>
--EXPECT--
Done
