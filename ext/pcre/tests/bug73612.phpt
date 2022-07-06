--TEST--
Bug #73612 (preg_*() may leak memory)
--FILE--
<?php
$obj = new stdClass;
$obj->obj = $obj;
preg_match('/./', 'x', $obj);

$obj = new stdClass;
$obj->obj = $obj;
preg_replace('/./', '', 'x', -1, $obj);

$obj = new stdClass;
$obj->obj = $obj;
preg_replace_callback('/./', 'count', 'x', -1, $obj);

$obj = new stdClass;
$obj->obj = $obj;
preg_replace_callback_array(['/./' => 'count'], 'x', -1, $obj);

$obj = new stdClass;
$obj->obj = $obj;
preg_filter('/./', '', 'x', -1, $obj);
?>
===DONE===
--EXPECT--
===DONE===
