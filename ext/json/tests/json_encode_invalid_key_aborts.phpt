--TEST--
json_encode() stops encoding after an object or array key fails to encode
--FILE--
<?php

class Value implements JsonSerializable
{
    public function jsonSerialize(): mixed
    {
        echo "jsonSerialize() called\n";
        return 1;
    }
}

eval('class Declared { public $' . "\xff" . ' = 1; public $next; }');

$dynamic = new stdClass;
$dynamic->{"\xff"} = 1;
$dynamic->next = new Value;

$declared = new Declared;
$declared->next = new Value;

$cases = [
    'array' => ["\xff" => 1, 'next' => new Value],
    'array, later NAN' => ["\xff" => 1, 'next' => NAN],
    'dynamic property' => $dynamic,
    'declared property' => $declared,
];

foreach ($cases as $name => $value) {
    echo "$name:\n";
    var_dump(json_encode($value), json_last_error_msg());
}

echo "partial output:\n";
var_dump(json_encode(["\xff" => 1, 'next' => 2], JSON_PARTIAL_OUTPUT_ON_ERROR), json_last_error_msg());

?>
--EXPECT--
array:
bool(false)
string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
array, later NAN:
bool(false)
string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
dynamic property:
bool(false)
string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
declared property:
bool(false)
string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
partial output:
string(15) "{"":1,"next":2}"
string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
