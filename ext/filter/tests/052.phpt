--TEST--
filter_var() / filter_var_array() and passed data integrity
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
function filter_cb($var)
{
  return 1;
}

$data = array ('bar' => array ('fu<script>bar', 'bar<script>fu') );
var_dump(filter_var($data, FILTER_SANITIZE_STRING, FILTER_FORCE_ARRAY));
var_dump($data);
var_dump(filter_var($data, FILTER_CALLBACK, array('options' => 'filter_cb')));
var_dump($data);
var_dump(filter_var_array($data, array('bar' => array('filter' => FILTER_CALLBACK, 'options' => 'filter_cb'))));
var_dump($data);
?>
--EXPECT--
array(1) {
  ["bar"]=>
  array(2) {
    [0]=>
    string(5) "fubar"
    [1]=>
    string(5) "barfu"
  }
}
array(1) {
  ["bar"]=>
  array(2) {
    [0]=>
    string(13) "fu<script>bar"
    [1]=>
    string(13) "bar<script>fu"
  }
}
array(1) {
  ["bar"]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(1)
  }
}
array(1) {
  ["bar"]=>
  array(2) {
    [0]=>
    string(13) "fu<script>bar"
    [1]=>
    string(13) "bar<script>fu"
  }
}
array(1) {
  ["bar"]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(1)
  }
}
array(1) {
  ["bar"]=>
  array(2) {
    [0]=>
    string(13) "fu<script>bar"
    [1]=>
    string(13) "bar<script>fu"
  }
}
