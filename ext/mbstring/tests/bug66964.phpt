--TEST--
Bug #66964 (mb_convert_variables() cannot detect recursion)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
$a[] = &$a;

try {
    var_dump(mb_convert_variables('utf-8', 'auto', $a));
} catch (\Error $e) {
    echo get_class($e) . ': ' . $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(mb_convert_variables('utf-8', 'utf-8', $a));
} catch (\Error $e) {
    echo get_class($e) . ': ' . $e->getMessage() . \PHP_EOL;
}

unset($a);
$a[] = '日本語テキスト';
$a[] = '日本語テキスト';
$a[] = '日本語テキスト';
$a[] = '日本語テキスト';
var_dump(mb_convert_variables('utf-8', 'utf-8', $a), $a);

$a[] = &$a;

try {
    var_dump(mb_convert_variables('utf-8', 'utf-8', $a), $a);
} catch (\Error $e) {
    echo get_class($e) . ': ' . $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
Error: Cannot handle recursive references
Error: Cannot handle recursive references
string(5) "UTF-8"
array(4) {
  [0]=>
  string(21) "日本語テキスト"
  [1]=>
  string(21) "日本語テキスト"
  [2]=>
  string(21) "日本語テキスト"
  [3]=>
  string(21) "日本語テキスト"
}
Error: Cannot handle recursive references