--TEST--
Compare getimagesize and getimagesizefromstring
--FILE--
<?PHP
$img = __DIR__ . '/test.gif';

$i1 = getimagesize($img);

$data = file_get_contents($img);

$i2 = getimagesizefromstring($data);

var_dump($i1);
var_dump($i2);
?>
--EXPECT--
array(7) {
  [0]=>
  int(120)
  [1]=>
  int(67)
  [2]=>
  int(1)
  [3]=>
  string(23) "width="120" height="67""
  ["bits"]=>
  int(7)
  ["channels"]=>
  int(3)
  ["mime"]=>
  string(9) "image/gif"
}
array(7) {
  [0]=>
  int(120)
  [1]=>
  int(67)
  [2]=>
  int(1)
  [3]=>
  string(23) "width="120" height="67""
  ["bits"]=>
  int(7)
  ["channels"]=>
  int(3)
  ["mime"]=>
  string(9) "image/gif"
}
