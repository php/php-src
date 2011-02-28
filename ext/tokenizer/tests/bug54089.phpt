--TEST--
Bug #54089 (token_get_all() does not stop after __halt_compiler)
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php
$code = "<?php __halt_compiler();\x01?>\x02";
$tokens = token_get_all($code);

var_dump($tokens);

$code = '';
foreach ($tokens as $t)
{
	$code .= isset($t[1]) ? $t[1] : $t;
}
var_dump($code);
?>
--EXPECTF--
array(2) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "<?php "
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(15) "__halt_compiler"
    [2]=>
    int(1)
  }
}
string(21) "<?php __halt_compiler"
