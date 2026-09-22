--TEST--
GH-21024 (UAF in json_encode() when jsonSerialize() frees the object)
--EXTENSIONS--
json
--FILE--
<?php
class Bar implements JsonSerializable {
    public function jsonSerialize(): mixed {
        global $ref;
        $ref = null;
        return ['k' => 1];
    }
}
$arr = [new Bar];
$ref = &$arr[0];
var_dump(json_encode($arr));
echo "survived\n";
?>
--EXPECT--
string(9) "[{"k":1}]"
survived
