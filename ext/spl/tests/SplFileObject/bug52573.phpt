--TEST--
Bug #52573 (SplFileObject::fscanf Segmentation fault)
--FILE--
<?php // test

$result = null;
$f = new SplFileObject(__FILE__, 'r');
var_dump($f->fscanf('<?php // %s', $result));
var_dump($result);
var_dump($f->fscanf('<?php // %s'));
?>
--EXPECT--
int(1)
string(4) "test"
array(1) {
  [0]=>
  NULL
}
