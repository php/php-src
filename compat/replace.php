<?php

/*
	Tool for syntactic replacements for <str_size_and_int64>
	Autor: ab@php.net
*/

/* Options stuff*/
$shortopts = NULL;
$longopts = array('type:', 'custom:', 'help::', 'reverse', 'zpp', 'zpp-compat', 'macros', 'zpp-vars');

$options = getopt($shortopts, $longopts);

$repl_type = isset($options['type']) ? $options['type'] : 'ext';
$custom_defs_fname = isset($options['custom']) ? $options['custom'] : NULL;
$fname = (count($argv) < 2) ? NULL : $argv[count($argv)-1];
$reverse_replace = isset($options['reverse']);
$replace_zpp = isset($options['zpp']);
$replace_zpp_compat = isset($options['zpp-compat']);
$replace_macros = isset($options['macros']);
$replace_zpp_vars = isset($options['zpp-vars']);

/* Options validation */
if (isset($options['help'])) {
	print_usage(0);
} else if (!in_array($repl_type, array('ext', 'zend_ext'))) {
	print_error("Invalid replacement type '$repl_type'");
} else if (!$replace_macros && !($replace_zpp || $replace_zpp_compat) && ! $replace_zpp_vars) {
	echo "No replacement options specified, nothing to do." . PHP_EOL . PHP_EOL;
	print_usage(3);
} else if ($replace_zpp && $replace_zpp_compat) {
	print_error("--zpp and --zpp-compat cannot be used at the same time");
} else if (!$fname || !file_exists($fname)) {
	print_error("Invalid filename '$fname'");
/*} else if (!is_null($custom_defs_fname) && !file_exists($custom_defs_fname)) {
	print_error("Invalid custom replacement definitions file");*/
} else if (!is_null($custom_defs_fname)) {
	/* XXX uncomment the above when it's implemented */
	print_error("Processing of custom replacement definitions is not implemented yet, feel free to supply a patch");
}


/* Process file */
$file_contents = file_get_contents($fname);

/* replace zpp variable types */
/* XXX handle more complicated cases */
if ($replace_zpp_vars && !$reverse_replace) {
	$ptr = array(
		"/zend_parse_parameters\(.*\s+TSRMLS_CC,\s*\"([a-zA-Z\*+\|\/!]+)\"\s*,\s*(.*)\)/msxU",
		"/zend_parse_parameters_ex\(.*,\s*.*\s+TSRMLS_CC,\s*\"([a-zA-Z\*+\|\/!]+)\"\s*,\s*(.*)\)/msxU",
		"/zend_parse_method_parameters\(.*\s+TSRMLS_CC,\s*.*,\s*\"([a-zA-Z\*+\|\/!]+)\"\s*,\s*(.*)\)/msxU",
		"/zend_parse_method_parameters_ex\(.*,\s*.*\s+TSRMLS_CC,\s*.*,\s*\"([a-zA-Z\*+\|\/!]+)\"\s*,\s*(.*)\)/msxU",
	);

	$vars = array();

	foreach ($ptr as $p) {
		/* XXX Warn about the cases we could find even if we don't replace them */
		if (0 < preg_match_all($p, $file_contents, $m)) {
			/* get specs and var names in one array */
			for ($i = 0; $i < count($m[0]); $i++) {
				$vars[$m[1][$i]] = array();

				$tmp = explode(',', str_replace(' ', '', $m[2][$i]));
				if (!is_array($tmp)) {
					echo "WARNING: parse error, zpp spec exist but var names couldn't be parsed" . PHP_EOL;	
					continue;
				}

				foreach ($tmp as $it) {
					while ($it && !preg_match(',^[a-z0-9_]+$,i', $it)) {
						$it = substr($it, 1);
					}
					$vars[$m[1][$i]][] = $it;
				}
			}
		}
	}

	/* compute var ingexes */
	$var_idx = array();
	/* XXX this loop might need to be fixed at some places! */
	foreach ($vars as $spec => $var) {
		$var_idx[$spec] = array();
		$var_idx[$spec]['php_int_t'] = array();
		$var_idx[$spec]['php_size_t'] = array();
		for ($i = 0, $pos = 0; $i < strlen($spec); $i++, $pos++) {
			switch($spec[$i]) {
				case 'l':
				case 'L':
					$var_idx[$spec]['php_int_t'][] = $pos;
					break;
				case 's':
				case 'p':
					$pos++;
					$var_idx[$spec]['php_size_t'][] = $pos;
					break;

				case 'a':
				case 'A':
				case 'b':
				case 'C':
				case 'd':
				case 'h':
				case 'H':
				case 'o':
				case 'r':
				case 'Z':
				case 'z':
				case '*':
				case '+':
					break;
				case '|':
				case '/':
				case '!':
					$pos--;
					break;

				case 'f':
				case 'O':
					$pos++;
					break;

				default:
					echo "WARNING: unknown zpp spec '" . $spec[$i] . "'" . PHP_EOL;
					break;
			}
		}
		if (empty($var_idx)) {
			unset($var_idx);
		}
	}

	/* got indexes and got vars, 1st simple case to match */
	$search_replace = array();
	foreach ($var_idx as $spec => $data) {
		/* handle direct matches with single variable declaration */
		foreach ($data['php_int_t'] as $idx) {
			$var = $vars[$spec][$idx];
			$cnt = preg_match(",(long\s+$var.*;),msxU", $file_contents, $m);
			if (1 == $cnt) {
				/* XXX probably better to use preg_replace instead of str_replace here */
				$search_replace[$m[1]] = str_replace('long', 'php_int_t', $m[1]);
			} else {
				echo "WARNING: variable $var needs to be php_int_t but couldn't be automatically replaced" . PHP_EOL;
			}
		}
		foreach ($data['php_size_t'] as $idx) {
			$var = $vars[$spec][$idx];
			$cnt = preg_match(",(int\s+$var.*;),msxU", $file_contents, $m);
			if (1 == $cnt) {
				/* XXX probably better to use preg_replace instead of str_replace here */
				$search_replace[$m[1]] = str_replace('int', 'php_size_t', $m[1]);;
			} else {
				echo "WARNING: variable $var needs to be php_size_t but couldn't be automatically replaced" . PHP_EOL;
			}
		}
	}
	$file_contents = str_replace(array_keys($search_replace), array_values($search_replace), $file_contents);

	/* XXX more cases here to go, var_dump($vars, var_idx); to see what data does already exist at this point */

} else if ($reverse_replace) {
	echo "WARNING: reverse replace for zpp vars isn't implemented, feel free to supply patch" . PHP_EOL;
}


/* simple macro replacements */
if ($replace_macros) {
	$replacements = array(
		/*'ZEND_SIZE_MAX_LONG'                     => 'ZEND_SIZE_MAX',
		'ZEND_SIZE_MAX_INT'                      => 'ZEND_SIZE_MAX',*/
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

	if ($reverse_replace) {
		/* cleanup also mess when convering back and forth */
		$replacements['INTERNAL'] = 'LONGERNAL';
		$replacements['INTERACTIVE'] = 'LONGERACTIVE';
		$replacements['INTEGER'] = 'LONGEGER';
		$file_contents = str_replace(array_values($replacements), array_keys($replacements), $file_contents);
	} else {
		$file_contents = str_replace(array_keys($replacements), array_values($replacements), $file_contents);
	}
}

/* zpp spec replacements */
if ($replace_zpp || $replace_zpp_compat) {
	$zpp_map = array(
		'l' => 'i',
		'L' => 'I',
		'p' => 'P',
		's' => 'S',
	);

	$ptr = array(
		"/zend_parse_parameters\(.*\s+TSRMLS_CC,\s*\"([a-zA-Z\*+\|\/!]+)\"/msxU",
		"/zend_parse_parameters_ex\(.*,\s*.*\s+TSRMLS_CC,\s*\"([a-zA-Z\*+\|\/!]+)\"/msxU",
		"/zend_parse_method_parameters\(.*\s+TSRMLS_CC,\s*.*,\s*\"([a-zA-Z\*+\|\/!]+)\"/msxU",
		"/zend_parse_method_parameters_ex\(.*,\s*.*\s+TSRMLS_CC,\s*.*,\s*\"([a-zA-Z\*+\|\/!]+)\"/msxU",
	);

	$raw_specs = $specs = array();

	foreach ($ptr as $p) {
		if (0 < preg_match_all($p, $file_contents, $m)) {
			$raw_specs = array_merge($raw_specs, $m[1]);
		}
	}

	$raw_specs = array_unique($raw_specs);

	foreach ($raw_specs as $item) {
		if ($reverse_replace) {
			$rep = str_replace(array_values($zpp_map), array_keys($zpp_map), $item);
			if ($rep != $item) {
				$specs['"' . $rep . '"'] = '"' . $item .'"';
			}
		} else {
			$rep = str_replace(array_keys($zpp_map), array_values($zpp_map), $item);
			if ($rep != $item) {
				$specs['"' . $item . '"'] = '"' . $rep .'"';
			}
		}

	}
	
	if ($replace_zpp_compat) {
		if ($reverse_replace) {
			echo "WARNING: reverse replace with --zpp-compat is ommited";
		} else {
			foreach ($specs as $spec0 => $spec1) {
				$specs[$spec0] = "ZPP_FMT_COMPAT($spec0, $spec1)";
			}
			$file_contents = str_replace(array_keys($specs), array_values($specs), $file_contents);
		}
	} else {

		if ($reverse_replace) {
			$file_contents = str_replace(array_values($specs), array_keys($specs), $file_contents);
		} else {
			$file_contents = str_replace(array_keys($specs), array_values($specs), $file_contents);
		}
	}
}


if (false === file_put_contents($fname, $file_contents)) {
	print_error("couldnt write into '$fname'");
}

/* Helpers */
function print_usage($code = 0)
{
	echo "Replacement tool for old vs. new macro names and more." . PHP_EOL;
	echo "Usage: replace.php [OPTIONS] file" . PHP_EOL;
	echo "  --custom      Path to custom replacement definitions file, optional." . PHP_EOL;
	echo "  --reverse     Replace the new names with the old ones, so reverse, optional." . PHP_EOL;
	echo "  --macros      Replace old vs. new macro names, optional." . PHP_EOL;
	echo "  --zpp         Replace the zpp format specs, optional." . PHP_EOL;  
	echo "  --zpp-compat  Same as --zpp but will add both old and new specs for compat, optional." . PHP_EOL;  
	echo "  --zpp-vars    Do the best and replace the datatypes on zpp vars, might go insane, optional." . PHP_EOL;
	echo "  --help        This help." . PHP_EOL;
	echo PHP_EOL;
	echo "Example: " . PHP_EOL;
	echo "    php replace.php --zpp --macros path/to/file" . PHP_EOL;
	echo "    php replace.php --zpp --macros --reverse path/to/file" . PHP_EOL;
	echo "    php replace.php --zpp--compat --macros path/to/file" . PHP_EOL;
	echo PHP_EOL;

	exit($code);
}

function print_error($str)
{
	echo "ERROR: " . $str . PHP_EOL;
	exit(3);
}

exit(0);

