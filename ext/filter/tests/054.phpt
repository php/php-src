--TEST--
filter_var_array() - using the add_empty option
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php

$data = array('foo' => 123);

var_dump(
    filter_var_array($data, array('foo' => array('filter' => FILTER_DEFAULT), 'bar' => array('filter' => FILTER_DEFAULT)), false),
    filter_var_array($data, array('foo' => array('filter' => FILTER_DEFAULT), 'bar' => array('filter' => FILTER_DEFAULT)))
);

?>
--EXPECT--
array(1) {
  ["foo"]=>
  string(3) "123"
}
array(2) {
  ["foo"]=>
  string(3) "123"
  ["bar"]=>
  NULL
}
