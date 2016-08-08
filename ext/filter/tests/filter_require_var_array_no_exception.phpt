--TEST--
Simple filter_require_var_array() no exception tests
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--INI--
error_reporting=-1
--FILE--
<?php
$data = array(
    'product_id'    => 'libgd<script>',
    'component'     => '10',
    'versions'      => '2.0.33',
    'testscalar'    => array('2', '23', '10', '12'),  // This is validation error
    'testarray'     => '2',
);

$args = array(
    'product_id'   => FILTER_SANITIZE_ENCODED,
    'component'    => array('filter'    => FILTER_VALIDATE_INT,
                            'flags'     => FILTER_FORCE_ARRAY,
                            'options'   => array('min_range' => 1, 'max_range' => 10)
                           ),
    'versions'     => FILTER_SANITIZE_ENCODED,
    'doesnotexist' => FILTER_VALIDATE_INT, // This is validation error w/o add_empty flag
    'testscalar'   => array(
                            'filter' => FILTER_VALIDATE_INT,
                            'flags'  => FILTER_REQUIRE_SCALAR,
                           ),
    'testarray'    => array(
                            'filter' => FILTER_VALIDATE_INT,
                            'flags'  => FILTER_FORCE_ARRAY,
                           )

);

try {
	var_dump(filter_var_array($data, $args)); // Should pass
	var_dump(filter_require_var_array($data, $args, FILTER_OPTS_DISABLE_EXCEPTION)); // Should fail
} catch (UnexpectedValueException $e) {
	var_dump($e->getMessage());
	var_dump(filter_get_invalid_key());
}

// Fix data so that 'testscalar' validates
$data['testscalar'] = '9999';
try {
	var_dump(filter_require_var_array($data, $args, FILTER_OPTS_ADD_EMPTY)); // Should pass
	var_dump(filter_require_var_array($data, $args, FILTER_OPTS_DISABLE_EXCEPTION)); // Try w/o add_empty flag. Should fail.
} catch (UnexpectedValueException $e) {
	var_dump($e->getMessage());
	var_dump(filter_get_invalid_key());
}
?>
--EXPECT--
array(6) {
  ["product_id"]=>
  string(17) "libgd%3Cscript%3E"
  ["component"]=>
  array(1) {
    [0]=>
    int(10)
  }
  ["versions"]=>
  string(6) "2.0.33"
  ["doesnotexist"]=>
  NULL
  ["testscalar"]=>
  bool(false)
  ["testarray"]=>
  array(1) {
    [0]=>
    int(2)
  }
}
bool(false)
array(6) {
  ["product_id"]=>
  string(17) "libgd%3Cscript%3E"
  ["component"]=>
  array(1) {
    [0]=>
    int(10)
  }
  ["versions"]=>
  string(6) "2.0.33"
  ["doesnotexist"]=>
  NULL
  ["testscalar"]=>
  int(9999)
  ["testarray"]=>
  array(1) {
    [0]=>
    int(2)
  }
}
bool(false)
