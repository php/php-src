--TEST--
#[\Override]: Properties: valid promoted property
--FILE--
<?php

interface I {
    public mixed $i { get; }
}

class C implements I {
    public function __construct(
        #[\Override]
        public mixed $i,
    ) {}
}

echo "Done";

?>
--EXPECT--
Done
