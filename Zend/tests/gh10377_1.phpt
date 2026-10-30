--TEST--
GH-10377 (Unable to have an anonymous readonly class) - usage variation: dynamic properties attribute
--FILE--
<?php

$readonly_anon = new #[AllowDynamicProperties] readonly class {
    public int $field;
    function __construct() {
        $this->field = 2;
    }
};

?>
--EXPECTF--
Fatal error: Cannot apply #[AllowDynamicProperties] to readonly class class@anonymous in %s on line %d
