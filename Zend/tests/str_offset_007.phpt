--TEST--
string offset 007 indirect string modification by error handler
--FILE--
<?php
set_error_handler(function($code, $msg) {
    echo "Err: $msg\n";
    $GLOBALS['a']='';
});
$a=['a'];
$a[0][$d]='b';
var_dump($a);
?>
--EXPECT--
array(1) {
  [0]=>
  string(1) "b"
}
Err: Undefined variable $d
Err: String offset cast occurred
