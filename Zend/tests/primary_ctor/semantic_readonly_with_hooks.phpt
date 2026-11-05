--TEST--
Primary constructors: private(set) hooked properties can be semantically readonly
--FILE--
<?php
class Temperature(
    public private(set) float $celsius {
        set {
            if ($value < -273.15) {
                throw new ValueError('below absolute zero');
            }
            $this->celsius = $value;
        }
    }
) {}

$t = new Temperature(20.0);
var_dump($t->celsius);

try {
    $t->celsius = 21.0;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    new Temperature(-300.0);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
float(20)
Cannot modify private(set) property Temperature::$celsius from global scope
below absolute zero
