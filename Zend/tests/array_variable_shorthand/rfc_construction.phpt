--TEST--
Array variable shorthand construction examples from the RFC
--FILE--
<?php
$x = 10;
$y = 20;
$point = [:$x, :$y];
echo json_encode($point), "\n";
echo json_encode(array(:$x, :$y)), "\n";

$name = 'Weilin';
$age = 26;
$user = ['id' => 1, :$name, :$age];
echo json_encode($user), "\n";
$values = [10, 20];
$result = [...$values, :$name,];
echo json_encode($result), "\n";

$user = array('id' => 1, :$name);
echo json_encode($user), "\n";
$record = ['user' => array(:$name)];
echo json_encode($record), "\n";

$Name = 'upper';
$name = 'lower';
echo json_encode([ : $Name, :$name]), "\n";

$name = 'new';
echo json_encode(['name' => 'old', :$name]), "\n";
echo json_encode([:$name, 'name' => 'later']), "\n";

$condition = false;
$baz = 'fallback';
echo json_encode([$condition ? 'bar' :$baz]), "\n";

$age = 26;
$name = 'Weilin';
echo json_encode([$age, :$name]), "\n";

$name = 'before';
$byValue = [:$name];
$byReference = ['name' => &$name];
$name = 'after';
echo $byValue['name'], "\n";
echo $byReference['name'], "\n";

$unkeyed = [$name = 'Weilin'];
$keyed = [:$name];
echo json_encode($unkeyed), "\n";
echo json_encode($keyed), "\n";
?>
--EXPECT--
{"x":10,"y":20}
{"x":10,"y":20}
{"id":1,"name":"Weilin","age":26}
{"0":10,"1":20,"name":"Weilin"}
{"id":1,"name":"Weilin"}
{"user":{"name":"Weilin"}}
{"Name":"upper","name":"lower"}
{"name":"new"}
{"name":"later"}
["fallback"]
{"0":26,"name":"Weilin"}
before
after
["Weilin"]
{"name":"Weilin"}
