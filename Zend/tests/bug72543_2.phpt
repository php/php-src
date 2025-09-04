--TEST--
Bug #72543.2 (different references behavior comparing to PHP 5)
--FILE--
<?php
$arr = [];
$arr[0] = null;
$ref =& $arr[0];
unset($ref);
$arr[0][$arr[0]] = null;
var_dump($arr);
?>
--EXPECTF--
Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d
array(1) {
  [0]=>
  array(1) {
    [""]=>
    NULL
  }
}
