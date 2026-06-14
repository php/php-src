--TEST--
#[\Override]: Properties: invalid promoted property
--FILE--
<?php

class C {
    public function __construct(
        #[\Override]
        public mixed $c,
    ) {}
}

echo "Done";

?>
--EXPECTF--
Fatal error: C::$c has #[\Override] attribute, but no matching parent property exists in %s on line %d
