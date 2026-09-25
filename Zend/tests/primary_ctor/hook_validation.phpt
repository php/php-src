--TEST--
Primary constructors: a set hook on a promoted parameter validates during construction
--FILE--
<?php
class Temperature(
    public float $celsius {
        set {
            if ($value < -273.15) {
                throw new ValueError('below absolute zero');
            }
            $this->celsius = $value;
        }
    }
) {}

var_dump((new Temperature(20.0))->celsius);

try {
    new Temperature(-300.0);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
float(20)
below absolute zero
