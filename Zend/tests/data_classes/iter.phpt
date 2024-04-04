--TEST--
Iteration of data classes
--FILE--
<?php

#[AllowDynamicProperties]
data class Box {
    public $value;
}

$box = new Box();
$box->value = 1;
$copy = $box;

foreach ($box as $value) {
    var_dump($value);
}

foreach ($box as $prop => &$value) {
    if ($box->value === 1) {
        $box->dynamic = 1;
    }
    $value++;
    var_dump($prop, $value);
}

?>
--EXPECT--
int(1)
string(5) "value"
int(2)
string(7) "dynamic"
int(2)
