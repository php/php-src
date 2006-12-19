--TEST--
input_get_args() filter not reseted between elements
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
$data = array(
    'product_id'    => 'libgd<script>',
    'component'     => '10dhsajkkdhk <do>',
    'versions'      => '2.0.33',
    'testscalar'    => array('2','23','10','12'),
    'testarray'     => '2',
);

$args = array(
    'product_id'    => FILTER_SANITIZE_ENCODED,
    'component'     => array('flags'    => FILTER_FORCE_ARRAY,
                             'options'  => array("min_range"=>1, "max_range"=>10)
                        ),
    'versions'      => array(
                            'filter' => FILTER_SANITIZE_ENCODED,
                            'flags'  => FILTER_REQUIRE_SCALAR,
                            ),
    'doesnotexist'  => FILTER_VALIDATE_INT,
    'testscalar'    => FILTER_VALIDATE_INT,
    'testarray' => array(
                            'filter' => FILTER_VALIDATE_INT,
                            'flags'  => FILTER_FORCE_ARRAY,
                        )

);
$out = filter_var_array($data, $args);
var_dump($out);
?>
--EXPECTF--	
array(6) {
  ["product_id"]=>
  string(17) "libgd%3Cscript%3E"
  ["component"]=>
  array(1) {
    [0]=>
    string(17) "%s"
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
