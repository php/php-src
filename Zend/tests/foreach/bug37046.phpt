--TEST--
Bug #37046 (foreach breaks static scope)
--FILE--
<?php
function s() {
  static $storage = array(array('x', 'y'));
  return $storage[0];
}

foreach (s() as $k => $function) {
  echo "op1 $k\n";
  if ($k == 0) {
    foreach (s() as $k => $function) {
      echo "op2 $k\n";
    }
  }
}
?>
--EXPECT--
op1 0
op2 0
op2 1
op1 1
