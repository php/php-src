--TEST--
GH-16725: Incorrect access check for non-hooked props in hooked object iterator
--FILE--
<?php

class C implements JsonSerializable
{
    private string $prop1 { get => 'bar'; }

    public function __construct(
        private string $prop2,
    ) {}

    public function jsonSerialize(): mixed {
        return get_object_vars($this);
    }
}

$obj = new C('foo');
var_dump(get_object_vars($obj));
echo json_encode($obj);

?>
--EXPECT--
array(0) {
}
{"prop1":"bar","prop2":"foo"}
