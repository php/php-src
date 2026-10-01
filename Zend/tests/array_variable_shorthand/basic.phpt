--TEST--
Array variable shorthand in array construction and destructuring
--FILE--
<?php
$x = 10;
$y = 20;
echo json_encode([:$x, :$y]), "\n";
echo json_encode([: $x, 'x' => 30, :$y]), "\n";
echo json_encode(array(:$x, : $y)), "\n";
$condition = false;
$baz = 7;
echo json_encode([$condition ? 'bar' :$baz]), "\n";
echo json_encode([$condition ?:$baz]), "\n";
echo json_encode([$condition ? 'bar' :$baz, :$x]), "\n";

$X = 40;
echo json_encode([:$x, :$X]), "\n";
echo json_encode([$x = $y]), "\n";

$point = ['x' => 11, 'y' => 22];
[:$x, :$y] = $point;
var_dump($x, $y);
list(:$x, :$y) = $point;
var_dump($x, $y);

foreach ([['x' => 1, 'y' => 2], ['x' => 3, 'y' => 4]] as [:$x, :$y]) {
    echo "$x:$y\n";
}
foreach (array($point) as list(:$x, :$y)) {
    echo "$x:$y\n";
}

['outer' => [:$x]] = ['outer' => ['x' => 5]];
var_dump($x);
list('outer' => list(:$x)) = array('outer' => array('x' => 6));
var_dump($x);
echo json_encode(array('outer' => [:$x])), "\n";
?>
--EXPECT--
{"x":10,"y":20}
{"x":30,"y":20}
{"x":10,"y":20}
[7]
[7]
{"0":7,"x":10}
{"x":10,"X":40}
[20]
int(11)
int(22)
int(11)
int(22)
1:2
3:4
11:22
int(5)
int(6)
{"outer":{"x":6}}
