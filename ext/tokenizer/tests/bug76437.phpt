--TEST--
Bug #76437 (token_get_all with TOKEN_PARSE flag fails to recognise close tag)
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php
$tests = [
    ['<?=$a?>', 0],
    ['<?php echo 2; ?>', 6],
    ["<?php echo 2; ?>\n", 6],
];
foreach ($tests as [$code, $index]) {
    $open_tag1 = token_get_all($code)[$index];
    $open_tag2 = token_get_all($code, TOKEN_PARSE)[$index];
    var_dump($open_tag1);
    var_dump($open_tag1 === $open_tag2);
}
?>
--EXPECT--
array(3) {
  [0]=>
  int(380)
  [1]=>
  string(3) "<?="
  [2]=>
  int(1)
}
bool(true)
array(3) {
  [0]=>
  int(381)
  [1]=>
  string(2) "?>"
  [2]=>
  int(1)
}
bool(true)
array(3) {
  [0]=>
  int(381)
  [1]=>
  string(3) "?>
"
  [2]=>
  int(1)
}
bool(true)
