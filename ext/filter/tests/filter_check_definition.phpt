--TEST--
validate_check_definition() tests 
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
// Good def
	$def = array(
		'product_id'   => FILTER_SANITIZE_ENCODED,
		'component'    => array(
			'filter'      => FILTER_VALIDATE_INT,
			'flags'       => FILTER_FORCE_ARRAY, 
			'options'     => array('min_range' => 1, 'max_range' => 10)
		),
		'versions'     => FILTER_SANITIZE_ENCODED,
		'doesnotexist' => FILTER_VALIDATE_INT,
		'1234'         => FILTER_SANITIZE_ENCODED,
		5678           => FILTER_SANITIZE_ENCODED,
		'testscalar'   => array(
			'filter'      => FILTER_VALIDATE_INT,
			'flags'       => FILTER_REQUIRE_SCALAR,
		),
		'testarray'    => array(
			// multiple filters
			array(
				'filter' => FILTER_VALIDATE_INT,
				'flags'  => FILTER_FORCE_ARRAY,
			),
			array(
				'filter' => FILTER_VALIDATE_FLOAT,
				'flags'  => FILTER_FORCE_ARRAY,
			),
		),
		'name'        => array(
			array(
				'filter' => FILTER_VALIDATE_STRING,
				'flags'  => FILTER_FLAG_STRING_ALNUM | FILTER_FLAG_STRING_ALLOW_NEWLINE,
				'options' => array(
					'min_bytes' => 10,
					'max_bytes' => 1000,
					'encoding' => FILTER_STRING_ENCODING_UTF8,
				),
			),
		),
		1234         => FILTER_DEFAULT,
	);

var_dump(filter_check_definition($def));

//Bad def
	$def = array(
		FILTER_SANITIZE_ENCODED,
		'component'    => array(
			'filters'   => FILTER_VALIDATE_INT,
			'flag'      => FILTER_FORCE_ARRAY, 
			'option'    => array('min_rang' => 1, 'max_rangee' => 10)
		),
		'versions'     => 'abc',
		TRUE => FILTER_VALIDATE_INT,
		1234   => array(
			'filter' => FILTER_VALIDATE_INT,
			'flags'  => FILTER_REQUIRE_SCALAR,
		),
		'testarray'    =>array(
			array(
				'filter' => array(FILTER_VALIDATE_INT),
				'flags'  => -100,
			),
			array(
				'filter' => "FILTER_VALIDATE_FLOAT",
				'flags'  => "FILTER_FORCE_ARRAY",
			),
			'foo' => 'bar',
		)
	);

var_dump(filter_check_definition($def));

echo "Done\n";
?>
--EXPECTF--
bool(true)

Warning: filter_check_definition(): Invalid spec: Found invalid key (filters) in %s on line 72

Warning: filter_check_definition(): Invalid spec: Found invalid key (flag) in %s on line 72

Warning: filter_check_definition(): Invalid spec: Found invalid key (option) in %s on line 72

Warning: filter_check_definition(): Invalid option value. Option should be array or long (key=versions) in %s on line 72

Warning: filter_check_definition(): Invalid spec: 'filter' data type is not long in %s on line 72

Warning: filter_check_definition(): Invalid spec: 'flags' cannot be negative in %s on line 72

Warning: filter_check_definition(): Invalid spec: 'filter' data type is not long in %s on line 72

Warning: filter_check_definition(): Invalid spec: 'flags' data type is not long in %s on line 72

Warning: filter_check_definition(): Invalid spec: Found invalid key (foo) in %s on line 72
bool(false)
Done
