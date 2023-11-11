--TEST--
json_encode() with JSON_PRETTY_PRINT on declared properties
--FILE--
<?php
#[AllowDynamicProperties]
class MyClass {
    public $x;
    public $y;
    public function __construct($x = 123, $y = []) {
        $this->x = $x;
        $this->y = $y;
    }
}

class HasNoProperties {}

echo json_encode(new HasNoProperties()), "\n";
echo json_encode(new HasNoProperties(), JSON_PRETTY_PRINT), "\n";

echo json_encode(new MyClass()), "\n";
echo json_encode(new MyClass(), JSON_PRETTY_PRINT), "\n";
$obj = new MyClass();
$obj->dynamic = new MyClass(null, []);
echo json_encode($obj), "\n";
echo json_encode($obj, JSON_PRETTY_PRINT), "\n";
$obj = new MyClass();
unset($obj->y);
echo json_encode($obj), "\n";
echo json_encode($obj, JSON_PRETTY_PRINT), "\n";
unset($obj->x);
echo json_encode($obj), "\n";
echo json_encode($obj, JSON_PRETTY_PRINT), "\n";
?>
--EXPECT--
{}
{}
{"x":123,"y":[]}
{
    "x": 123,
    "y": []
}
{"x":123,"y":[],"dynamic":{"x":null,"y":[]}}
{
    "x": 123,
    "y": [],
    "dynamic": {
        "x": null,
        "y": []
    }
}
{"x":123}
{
    "x": 123
}
{}
{}
