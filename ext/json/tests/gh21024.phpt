--TEST--
GH-21024 (UAF in json_encode() when jsonSerialize()'s error handler frees the object)
--EXTENSIONS--
json
--FILE--
<?php
class Bar implements JsonSerializable {
    public function jsonSerialize(): mixed {
        echo $undefined;
        return ['k' => 1];
    }
}
$arr = [new Bar];
$ref = &$arr[0];
set_error_handler(function () use (&$ref) { $ref = null; });
var_dump(json_encode($arr));
echo "survived\n";
?>
--EXPECT--
string(9) "[{"k":1}]"
survived
