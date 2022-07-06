--TEST--
Bug #24652 (broken array_flip())
--FILE--
<?php
  /* This works */
  $f = array('7' => 0);
  var_dump($f);
  var_dump(array_key_exists(7, $f));
  var_dump(array_key_exists('7', $f));

  print "----------\n";
  /* This doesn't */
  $f = array_flip(array('7'));
  var_dump($f);
  var_dump(array_key_exists(7, $f));
  var_dump(array_key_exists('7', $f));
?>
--EXPECT--
array(1) {
  [7]=>
  int(0)
}
bool(true)
bool(true)
----------
array(1) {
  [7]=>
  int(0)
}
bool(true)
bool(true)
