--TEST--
Bug #26639 (mb_convert_variables() clutters variables beyond the references)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
$a = "‚ ‚¢‚¤‚¦‚¨";
$b = $a;
mb_convert_variables("EUC-JP", "Shift_JIS", $b);
debug_zval_dump($a);
debug_zval_dump($b);
unset($a);
unset($b);

$a = "‚ ‚¢‚¤‚¦‚¨";
$b = &$a;
mb_convert_variables("EUC-JP", "Shift_JIS", $b);
debug_zval_dump($a);
debug_zval_dump($b);
unset($a);
unset($b);

$a = "‚ ‚¢‚¤‚¦‚¨";
$b = array($a);
$c = $b;
mb_convert_variables("EUC-JP", "Shift_JIS", $c);
debug_zval_dump($b);
debug_zval_dump($c);
unset($a);
unset($b);
unset($c);

$a = "‚ ‚¢‚¤‚¦‚¨";
$b = array(&$a);
$c = $b;
mb_convert_variables("EUC-JP", "Shift_JIS", $c);
debug_zval_dump($b);
debug_zval_dump($c);
unset($a);
unset($b);
unset($c);

$a = "‚ ‚¢‚¤‚¦‚¨";
$b = array($a);
$c = &$b;
mb_convert_variables("EUC-JP", "Shift_JIS", $c);
debug_zval_dump($b);
debug_zval_dump($c);
unset($a);
unset($b);
unset($c);

$a = "‚ ‚¢‚¤‚¦‚¨";
$b = array(&$a);
$c = &$b;
mb_convert_variables("EUC-JP", "Shift_JIS", $c);
debug_zval_dump($b);
debug_zval_dump($c);
unset($a);
unset($b);
unset($c);

$a = array(array("‚ ‚¢‚¤‚¦‚¨"));
$b = $a;
$c = $b;
mb_convert_variables("EUC-JP", "Shift_JIS", $c);
debug_zval_dump($b);
debug_zval_dump($c);
unset($a);
unset($b);
unset($c);
?>
--EXPECTF--
string(10) "‚ ‚¢‚¤‚¦‚¨" refcount(%d)
string(10) "¤¢¤¤¤¦¤¨¤ª" refcount(%d)
string(10) "¤¢¤¤¤¦¤¨¤ª" refcount(%d)
string(10) "¤¢¤¤¤¦¤¨¤ª" refcount(%d)
array(1) refcount(%d){
  [0]=>
  string(10) "‚ ‚¢‚¤‚¦‚¨" refcount(%d)
}
array(1) refcount(%d){
  [0]=>
  string(10) "¤¢¤¤¤¦¤¨¤ª" refcount(%d)
}
array(1) refcount(%d){
  [0]=>
  &string(10) "‚ ‚¢‚¤‚¦‚¨" refcount(%d)
}
array(1) refcount(%d){
  [0]=>
  string(10) "¤¢¤¤¤¦¤¨¤ª" refcount(%d)
}
array(1) refcount(%d){
  [0]=>
  string(10) "¤¢¤¤¤¦¤¨¤ª" refcount(%d)
}
array(1) refcount(%d){
  [0]=>
  string(10) "¤¢¤¤¤¦¤¨¤ª" refcount(%d)
}
array(1) refcount(%d){
  [0]=>
  string(10) "¤¢¤¤¤¦¤¨¤ª" refcount(%d)
}
array(1) refcount(%d){
  [0]=>
  string(10) "¤¢¤¤¤¦¤¨¤ª" refcount(%d)
}
array(1) refcount(%d){
  [0]=>
  array(1) refcount(%d){
    [0]=>
    string(10) "‚ ‚¢‚¤‚¦‚¨" refcount(%d)
  }
}
array(1) refcount(%d){
  [0]=>
  array(1) refcount(%d){
    [0]=>
    string(10) "¤¢¤¤¤¦¤¨¤ª" refcount(%d)
  }
}
