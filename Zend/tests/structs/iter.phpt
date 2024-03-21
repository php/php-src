--TEST--
Iteration of structs
--FILE--
<?php

#[AllowDynamicProperties]
struct Box {
    public $value;
}

$box = new Box();
$box->value = 1;
$copy = $box;

echo "Iteration by value\n";

foreach ($box as $value) {
    var_dump($value);
}

echo "\nIteration by ref\n";

foreach ($box as $prop => &$value) {
    if ($prop === 'value' && $value === 1) {
        $box->dynamic = 1;
    }
    $value++;
    var_dump($value);
}

echo "\nIteration by ref on copy\n";

$copy2 = $copy;
$copy2Ref = &$copy2;

foreach ($copy2Ref as $prop => &$value) {
    $value++;
    var_dump($value);
}

echo "\nIteration by ref on ref with separation\n";

function &getBox() {
    global $box;
    $box2 = $box;
    return $box2;
}

foreach (getBox() as $prop => &$value) {
    $value++;
    var_dump($value);
}

echo "\nDone\n";

var_dump($box, $copy);

?>
--EXPECT--
Iteration by value
int(1)

Iteration by ref
int(2)
int(2)

Iteration by ref on copy
int(2)

Iteration by ref on ref with separation
int(3)
int(3)

Done
object(Box)#2 (2) {
  ["value"]=>
  int(2)
  ["dynamic"]=>
  int(2)
}
object(Box)#1 (1) {
  ["value"]=>
  int(1)
}
