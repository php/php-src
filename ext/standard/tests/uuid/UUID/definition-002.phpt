--TEST--
UUID::VARIANT_* constant definition
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

var_dump(
    UUID::VARIANT_NCS,
    UUID::VARIANT_RFC4122,
    UUID::VARIANT_MICROSOFT,
    UUID::VARIANT_FUTURE_RESERVED
);

?>
--EXPECT--
int(0)
int(1)
int(2)
int(3)
