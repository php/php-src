--TEST--
Array variable shorthand destructuring examples from the RFC
--FILE--
<?php
$point = ['x' => 10, 'y' => 20];
[:$x, :$y] = $point;
echo "$x:$y\n";
list(:$x, :$y) = $point;
echo "$x:$y\n";

$user = ['name' => 'Weilin', 'age' => 26];
[:$name, 'age' => $years] = $user;
echo "$name:$years\n";
list(:$name, 'age' => $years) = $user;
echo "$name:$years\n";

$rows = [['name' => 'Weilin']];
foreach ($rows as [:$name]) {
    echo "$name\n";
}

$user = array('id' => 1, 'name' => 'Weilin');
list(:$name) = $user;
echo "$name\n";
foreach (array($user) as list(:$name)) {
    echo "$name\n";
}

$record = ['user' => ['name' => 'Ada']];
list('user' => list(:$name)) = $record;
echo "$name\n";
['user' => [:$name]] = $record;
echo "$name\n";
?>
--EXPECT--
10:20
10:20
Weilin:26
Weilin:26
Weilin
Weilin
Weilin
Ada
Ada
