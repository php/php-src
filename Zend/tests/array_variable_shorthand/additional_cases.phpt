--TEST--
Array variable shorthand with explicit alternatives, repeated keys and named arguments
--FILE--
<?php
$object = new stdClass();
$object->name = 'property';
echo json_encode(['name' => $object->name]), "\n";

$name = 'first';
echo json_encode([:$name, :$name]), "\n";
$name = 'second';
echo json_encode(array('name' => 'old', :$name, 'name' => 'last')), "\n";

$values = [10, 20];
echo json_encode(array(...$values, :$name)), "\n";

$name = 'new';
echo json_encode([...['name' => 'old'], :$name]), "\n";
echo json_encode([:$name, ...['name' => 'later']]), "\n";

$object = new ArrayObject(['name' => 'Ada']);
[:$name] = $object;
echo "$name\n";
list(:$name) = $object;
echo "$name\n";

function receive_name($name) {
    return $name;
}
echo receive_name(name: $name), "\n";

$record = ['outer' => ['name' => 'nested']];
['outer' => [:$name]] = $record;
echo "$name\n";
?>
--EXPECT--
{"name":"property"}
{"name":"first"}
{"name":"last"}
{"0":10,"1":20,"name":"second"}
{"name":"new"}
{"name":"later"}
Ada
Ada
Ada
nested
