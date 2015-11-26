<?php
error_reporting(E_ALL|E_STRICT);
$data = array(
    'product_id'	=> 'product id<script>',
    'component'		=> '10',
    'versions'		=> '1.2.33',
	'testscalar'	=> array('2','23','10','12'),
	'testarray'		=> '2',
);

$args = array(
    'product_id'    => FILTER_SANITIZE_ENCODED,
    'component'     => array('filter'	=> FILTER_VALIDATE_INT,
							 'flags'	=> FILTER_FLAG_ARRAY,
							 'options'	=> array("min_range"=>1, "max_range"=>10)
						),

	/* equivalent of => FILTER_SANITIZE_ENCODED as SCALAR is
	 * the default mode
     */
    'versions'      => array(
                            'filter' => FILTER_SANITIZE_ENCODED,
                            'flags'  => FILTER_FLAG_SCALAR,
							),
	'doesnotexist'	=> FILTER_VALIDATE_INT,
	'testscalar'	=> FILTER_VALIDATE_INT,
	'testarray'	=> array(
							'filter' => FILTER_VALIDATE_INT,
							'flags'  => FILTER_FLAG_ARRAY,
						)

);

/*
The other INPUT_* can be used as well.
$myinputs = input_get_args($args, INPUT_POST);
*/
$myinputs = input_get_args($args, INPUT_DATA, $data);

var_dump($myinputs);

