<?php

/*
	Tool for syntactic replacements for <str_size_and_int64>
	Autor: ab@php.net
*/

/* Options stuff*/
$shortopts = NULL;
$longopts = array('type:', 'custom:', 'help::');

$options = getopt($shortopts, $longopts);

$repl_type = isset($options['type']) ? $options['type'] : 'ext';
$custom_defs_fname = isset($options['custom']) ? $options['custom'] : NULL;
$fname = $argv[count($argv)-1];

/* Options validation */
if (isset($options['help'])) {
	print_usage(0);
} else if (!in_array($repl_type, array('ext', 'zend_ext'))) {
	print_error("Invalid replacement type '$repl_type'");
} else if (!$fname || !file_exists($fname)) {
	print_error("Invalid filename '$fname'");
/*} else if (!is_null($custom_defs_fname) && !file_exists($custom_defs_fname)) {
	print_error("Invalid custom replacement definitions file");*/
} else if (!is_null($custom_defs_fname)) {
	/* XXX uncomment the above when it's implemented */
	print_error("Processing of custom replacement definitions is not implemented yet, feel free to supply a patch");
}


/* Process file */
$replacements = array(
	'ZEND_SIZE_MAX_LONG'                     => 'ZEND_SIZE_MAX',
	'ZEND_SIZE_MAX_INT'                      => 'ZEND_SIZE_MAX',
	'MAX_LENGTH_OF_LONG'                     => 'MAX_LENGTH_OF_ZEND_INT',
	'Z_STRLEN'                               => 'Z_STRSIZE',
	'Z_LVAL'                                 => 'Z_IVAL',
	'IS_LONG'                                => 'IS_INT',
	'ZVAL_LONG'                              => 'ZVAL_INT',
	'RETVAL_LONG'                            => 'RETVAL_INT',
	'RETURN_LONG'                            => 'RETURN_INT',
	'LITERAL_LONG'                           => 'LITERAL_INT',
	'REGISTER_LONG_CONSTANT'                 => 'REGISTER_INT_CONSTANT',
	'REGISTER_NS_LONG_CONSTANT'              => 'REGISTER_NS_INT_CONSTANT',
	'REGISTER_MAIN_LONG_CONSTANT'            => 'REGISTER_MAIN_INT_CONSTANT',
	'HASH_KEY_IS_LONG'                       => 'HASH_KEY_IS_INT',
	'convert_to_long'                        => 'convert_to_int',
	'add_assoc_long'                         => 'add_assoc_int',
	'add_property_long'                      => 'add_property_int',
	'zendi_convert_to_long'                  => 'zendi_convert_to_int',
	'zend_ini_long'                          => 'zend_ini_int',
	'ZEND_SIGNED_MULTIPLY_LONG'              => 'ZEND_SIGNED_MULTIPLY_INT',
	'zend_update_property_long'              => 'zend_update_property_int',
	'SET_VAR_LONG'                           => 'SET_VAR_INT',
	'zend_declare_class_constant_long'       => 'zend_declare_class_constant_int',
	'add_get_index_long'                     => 'add_get_index_int',
	'add_next_index_long'                    => 'add_next_index_int',
	'zend_declare_class_constant_long'       => 'zend_declare_class_constant_int',
	'zend_update_static_property_long'       => 'zend_update_static_property_int',
	'zend_register_long_constant'            => 'zend_register_int_constant',
	'add_index_long'                         => 'add_index_int',
	'zend_declare_property_long'             => 'zend_declare_property_int',
	'ZEND_MM_LONG_CONST'                     => 'ZEND_MM_INT_CONST',
	'REGISTER_PDO_CLASS_CONST_LONG'          => 'REGISTER_PDO_CLASS_CONST_INT',
	'PDO_LONG_PARAM_CHECK'                   => 'PDO_INT_PARAM_CHECK',
	'REGISTER_XMLREADER_CLASS_CONST_LONG'    => 'REGISTER_XMLREADER_CLASS_CONST_INT',
	'CALENDAR_DECL_LONG_CONST'               => 'CALENDAR_DECL_INT_CONST',
	'TIMEZONE_DECL_LONG_CONST'               => 'TIMEZONE_DECL_INT_CONST',
	'BREAKITER_DECL_LONG_CONST'              => 'BREAKITER_DECL_INT_CONST',
	'REGISTER_SPL_CLASS_CONST_LONG'          => 'REGISTER_SPL_CLASS_CONST_INT',
	'REGISTER_REFLECTION_CLASS_CONST_LONG'   => 'REGISTER_REFLECTION_CLASS_CONST_INT',
	'REGISTER_PHAR_CLASS_CONST_LONG'         => 'REGISTER_PHAR_CLASS_CONST_INT',
	'MAKE_LONG_ZVAL_INCREF'                  => 'MAKE_INT_ZVAL_INCREF',
	'MYSQLI_RETURN_INT_LONG'                 => 'MYSQLI_RETURN_INT_INT',
	'MYSQLI_MAP_PROPERTY_FUNC_LONG'          => 'MYSQLI_MAP_PROPERTY_FUNC_INT',
	'REGISTER_ZIP_CLASS_CONST_LONG'          => 'REGISTER_ZIP_CLASS_CONST_INT',
	'REGISTER_SNMP_CLASS_CONST_LONG'         => 'REGISTER_SNMP_CLASS_CONST_INT',
	'PHP_SNMP_LONG_PROPERTY_READER_FUNCTION' => 'PHP_SNMP_INT_PROPERTY_READER_FUNCTION',
	'ISC_LONG_MIN'                           => 'ISC_INT_MIN',
	'ISC_LONG_MAX'                           => 'ISC_INT_MAX',
	'long_min_digits'                        => 'int_min_digits',
	'zend_dval_to_lval'                      => 'zend_dval_to_ival',
	'ZEND_DVAL_TO_LVAL_CAST_OK'              => 'ZEND_DVAL_TO_IVAL_CAST_OK',
	'smart_str_append_long'                  => 'smart_str_append_int',
	'cfg_get_long'                           => 'cfg_get_int',
	'pdo_attr_lval'                          => 'pdo_attr_ival',
);

$code_old = file_get_contents($fname);
$code_new = str_replace(array_keys($replacements), array_values($replacements), $code_old);
if (false === file_put_contents($fname, $code_new)) {
	print_error("couldnt write into '$fname'");
}

/* Helpers */
function print_usage($code = 0)
{
	echo "Usage: replace.php [OPTIONS] file" . PHP_EOL;
	echo "  --custom  Path to custom replacement definitions file, optional." . PHP_EOL;
	echo "  --help    This help." . PHP_EOL;
	echo PHP_EOL;

	exit($code);
}

function print_error($str)
{
	echo $str . PHP_EOL;
	exit(3);
}

exit(0);

