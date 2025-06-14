--TEST--
Virtual promoted property in readonly class cannot have hooks
--FILE--
<?php

readonly class Test {
    public function __construct(
        public int $prop {
            get => 42;
        }
    ) {}
}

?>
--EXPECTF--
Fatal error: Hooked virtual properties may not be declared readonly in %s on line %d
