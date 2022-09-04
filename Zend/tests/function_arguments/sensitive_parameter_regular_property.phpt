--TEST--
The SensitiveParameter attribute leads to fatal error on non-promoted properties.
--FILE--
<?php

class WithSensitiveParameter
{
    #[SensitiveParameter]
    public string $prop;
}

?>
--EXPECTF--
Fatal error: Attribute "SensitiveParameter" cannot target property (allowed targets: parameter) in %ssensitive_parameter_regular_property.php on line 6
