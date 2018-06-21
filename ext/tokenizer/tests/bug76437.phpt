--TEST--
Bug #76437 (token_get_all with TOKEN_PARSE flag fails to recognise close tag)
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php
$open_tag1 = token_get_all('<?=$a?>')[0];
$open_tag2 = token_get_all('<?=$a?>', TOKEN_PARSE)[0];
var_dump($open_tag1);
var_dump($open_tag1 === $open_tag2);
$open_tag1 = token_get_all('<?php echo 2; ?>')[6];
$open_tag2 = token_get_all('<?php echo 2; ?>', TOKEN_PARSE)[6];
var_dump($open_tag1);
var_dump($open_tag1 === $open_tag2);
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
