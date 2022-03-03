--TEST--
unset() CV 11 (unset() of copy destroys original value)
--FILE--
<?php
$x = array("default"=>"ok");
var_dump($x);
$cf = $x;
unset($cf['default']);
var_dump($x);
echo "ok\n";
?>
--EXPECT--
array(1) {
  ["default"]=>
  string(2) "ok"
}
array(1) {
  ["default"]=>
  string(2) "ok"
}
ok
