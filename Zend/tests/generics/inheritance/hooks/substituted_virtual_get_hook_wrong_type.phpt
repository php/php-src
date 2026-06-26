--TEST--
Property hooks: virtual get hook returning a hardcoded value of the wrong type is rejected on the substituted view
--FILE--
<?php
class Box<T> {
    public T $value {
        get => "hardcoded";
    }
}

class IntBox extends Box<int> {}

$p = new Box();
var_dump($p->value);

$b = new IntBox();
try {
    $v = $b->value;
} catch (TypeError $e) {
    echo "caught: ", $e->getMessage(), "\n";
}
?>
--EXPECTF--
string(9) "hardcoded"
caught: Box::$value::get(): Return value must be of type int, string returned
