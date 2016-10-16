--TEST--
Bug #26639 (mb_convert_variables() clutters variables beyond the references)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
$orig = 'gqCCooKkgqaCqA=='; // SJIS あいうえお
$s = base64_decode($orig);

echo "====== Case 1 ======\n";
$a = $s;
$b = $a;
mb_convert_variables("UTF-8", "Shift_JIS", $b);
debug_zval_dump(base64_encode($a));
debug_zval_dump($b);
var_dump(base64_encode($a) == $orig);
var_dump($a == $b);
unset($a);
unset($b);

echo "====== Case 2 ======\n";
$a = $s;
$b = &$a;
mb_convert_variables("UTF-8", "Shift_JIS", $b);
debug_zval_dump($a);
debug_zval_dump($b);
var_dump($a == $s);
var_dump($a == $b);
unset($a);
unset($b);

echo "====== Case 3 ======\n";
$a = $s;
$b = array($a);
$c = $b;
mb_convert_variables("UTF-8", "Shift_JIS", $c);
debug_zval_dump($b);
debug_zval_dump($c);
var_dump($c == $b);
unset($a);
unset($b);
unset($c);

echo "====== Case 4 ======\n";
$a = $s;
$b = array(&$a);
$c = $b;
mb_convert_variables("UTF-8", "Shift_JIS", $c);
debug_zval_dump($b);
debug_zval_dump($c);
var_dump($c == $b);
unset($a);
unset($b);
unset($c);

echo "====== Case 5 ======\n";
$a = $s;
$b = array($a);
$c = &$b;
mb_convert_variables("UTF-8", "Shift_JIS", $c);
debug_zval_dump($b);
debug_zval_dump($c);
var_dump($c == $b);
unset($a);
unset($b);
unset($c);

echo "====== Case 6 ======\n";
$a = $s;
$b = array(&$a);
$c = &$b;
mb_convert_variables("UTF-8", "Shift_JIS", $c);
debug_zval_dump($b);
debug_zval_dump($c);
var_dump($c == $b);
unset($a);
unset($b);
unset($c);

echo "====== Case 7 ======\n";
$a = array(array($s));
$b = $a;
$c = $b;
mb_convert_variables("UTF-8", "Shift_JIS", $c);
debug_zval_dump($b);
debug_zval_dump($c);
var_dump($c == $b);
unset($a);
unset($b);
unset($c);
?>
--EXPECT--
====== Case 1 ======
string(16) "gqCCooKkgqaCqA==" refcount(1)
string(15) "あいうえお" refcount(2)
bool(true)
bool(false)
====== Case 2 ======
string(15) "あいうえお" refcount(2)
string(15) "あいうえお" refcount(2)
bool(false)
bool(true)
====== Case 3 ======
array(1) refcount(2){
  [0]=>
  string(10) "����������" refcount(3)
}
array(1) refcount(2){
  [0]=>
  string(15) "あいうえお" refcount(1)
}
bool(false)
====== Case 4 ======
array(1) refcount(2){
  [0]=>
  &string(15) "あいうえお" refcount(1)
}
array(1) refcount(2){
  [0]=>
  &string(15) "あいうえお" refcount(1)
}
bool(true)
====== Case 5 ======
array(1) refcount(2){
  [0]=>
  string(15) "あいうえお" refcount(1)
}
array(1) refcount(2){
  [0]=>
  string(15) "あいうえお" refcount(1)
}
bool(true)
====== Case 6 ======
array(1) refcount(2){
  [0]=>
  &string(15) "あいうえお" refcount(1)
}
array(1) refcount(2){
  [0]=>
  &string(15) "あいうえお" refcount(1)
}
bool(true)
====== Case 7 ======
array(1) refcount(3){
  [0]=>
  array(1) refcount(1){
    [0]=>
    string(10) "����������" refcount(2)
  }
}
array(1) refcount(2){
  [0]=>
  array(1) refcount(1){
    [0]=>
    string(15) "あいうえお" refcount(1)
  }
}
bool(false)