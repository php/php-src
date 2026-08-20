--TEST--
Primary constructors: hooked promoted properties cannot be readonly
--FILE--
<?php
readonly class Temperature(
    public float $celsius {
        set { $this->celsius = $value; }
    }
) {}
?>
--EXPECTF--
Fatal error: Hooked properties cannot be readonly in %s on line %d
