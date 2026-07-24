--TEST--
Autovivification of false to array with data clobbering by error handler
--FILE--
<?php
set_error_handler(function($code, $msg) {
    echo "Err: $msg\n";
    $GLOBALS['a']='';
});
$a=[!'a'];
$a[0][$d]='b';
var_dump($a);
?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    [""]=>
    string(1) "b"
  }
}
Err: Automatic conversion of false to array is deprecated
Err: Undefined variable $d
Err: Using null as an array offset is deprecated, use an empty string instead
