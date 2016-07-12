<?php
/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2007 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Nuno Lopes <nlopess@php.net>                                 |
  +----------------------------------------------------------------------+
*/

/* $Id$ */


define('REPORT_LEVEL', 1); // 0 reports less false-positives. up to level 5.
define('VERSION', '7.0');  // minimum is 7.0
define('PHPDIR', realpath(dirname(__FILE__) . '/../..'));


// be sure you have enough memory and stack for PHP. pcre will push the limits!
ini_set('pcre.backtrack_limit', 10000000);


// ------------------------ end of config ----------------------------


$API_params = array(
	'a' => array('zval**'), // array
	'A' => array('zval**'), // array or object
	'b' => array('zend_bool*'), // boolean
	'd' => array('double*'), // double
	'f' => array('zend_fcall_info*', 'zend_fcall_info_cache*'), // function
	'h' => array('HashTable**'), // array as an HashTable*
	'H' => array('HashTable**'), // array or HASH_OF(object)
	'l' => array('zend_long*'), // long
	//TODO 'L' => array('zend_long*, '), // long
	'o' => array('zval**'), //object
	'O' => array('zval**', 'zend_class_entry*'), // object of given type
	'P' => array('zend_string**'), // valid path
	'r' => array('zval**'), // resource
	'S' => array('zend_string**'), // string
	'z' => array('zval**'), // zval*
	'Z' => array('zval***') // zval**
	// 's', 'p', 'C' handled separately
);

/** reports an error, according to its level */
function error($str, $level = 0)
{
	global $current_file, $current_function, $line;

	if ($level <= REPORT_LEVEL) {
		if (strpos($current_file,PHPDIR) === 0) {
			$filename = substr($current_file, strlen(PHPDIR)+1);
		} else {
			$filename = $current_file;
		}
		echo $filename , " [$line] $current_function : $str\n";
	}
}


/** this updates the global var $line (for error reporting) */
function update_lineno($offset)
{
	global $lines_offset, $line;

	$left  = 0;
	$right = $count = count($lines_offset)-1;

	// a nice binary search :)
	do {
		$mid = intval(($left + $right)/2);
		$val = $lines_offset[$mid];

		if ($val < $offset) {
			if (++$mid > $count || $lines_offset[$mid] > $offset) {
				$line = $mid;
				return;
			} else {
				$left = $mid;
			}
		} else if ($val > $offset) {
			if ($lines_offset[--$mid] < $offset) {
				$line = $mid+1;
				return;
			} else {
				$right = $mid;
			}
		} else {
			$line = $mid+1;
			return;
		}
	} while (true);
}


/** parses the sources and fetches its vars name, type and if they are initialized or not */
function get_vars($txt)
{
	$ret =  array();
	preg_match_all('/((?:(?:unsigned|struct)\s+)?\w+)(?:\s*(\*+)\s+|\s+(\**))(\w+(?:\[\s*\w*\s*\])?)\s*(?:(=)[^,;]+)?((?:\s*,\s*\**\s*\w+(?:\[\s*\w*\s*\])?\s*(?:=[^,;]+)?)*)\s*;/S', $txt, $m, PREG_SET_ORDER);

	foreach ($m as $x) {
		// the first parameter is special
		if (!in_array($x[1], array('else', 'endif', 'return'))) // hack to skip reserved words
			$ret[$x[4]] = array($x[1] . $x[2] . $x[3], $x[5]);

		// are there more vars?
		if ($x[6]) {
			preg_match_all('/(\**)\s*(\w+(?:\[\s*\w*\s*\])?)\s*(=?)/S', $x[6], $y, PREG_SET_ORDER);
			foreach ($y as $z) {
				$ret[$z[2]] = array($x[1] . $z[1], $z[3]);
			}
		}
	}

//	if ($GLOBALS['current_function'] == 'for_debugging') { print_r($m);print_r($ret); }
	return $ret;
}


/** run diagnostic checks against one var. */
function check_param($db, $idx, $exp, $optional, $allow_uninit = false)
{
	global $error_few_vars_given;

	if ($idx >= count($db)) {
		if (!$error_few_vars_given) {
			error("too few variables passed to function");
			$error_few_vars_given = true;
		}
		return;
	} elseif ($db[$idx][0] === '**dummy**') {
		return;
	}

	if ($db[$idx][1] != $exp) {
		error("{$db[$idx][0]}: expected '$exp' but got '{$db[$idx][1]}' [".($idx+1).']');
	}

	if (!$optional && $db[$idx][2]) {
		error("not optional var is initialized: {$db[$idx][0]} [".($idx+1).']', 2);
	}
	if (!$allow_uninit && $optional && !$db[$idx][2]) {
		error("optional var not initialized: {$db[$idx][0]} [".($idx+1).']', 1);
	}
}

/** fetch params passed to zend_parse_params*() */
function get_params($vars, $str)
{
	$ret = array();
	preg_match_all('/(?:\([^)]+\))?(&?)([\w>.()-]+(?:\[\w+\])?)\s*,?((?:\)*\s*=)?)/S', $str, $m, PREG_SET_ORDER);

	foreach ($m as $x) {
		$name = $x[2];

		// little hack for last parameter
		if (strpos($name, '(') === false) {
			$name = rtrim($name, ')');
		}

		if (empty($vars[$name][0])) {
			error("variable not found: '$name'", 3);
			$ret[][] = '**dummy**';

		} else {
			$ret[] = array($name, $vars[$name][0] . ($x[1] ? '*' : ''), $vars[$name][1]);
		}

		// the end (yes, this is a little hack :P)
		if ($x[3]) {
			break;
		}
	}

//	if ($GLOBALS['current_function'] == 'for_debugging') { var_dump($m); var_dump($ret); }
	return $ret;
}


/** run tests on a function. the code is passed in $txt */
function check_function($name, $txt, $offset)
{
	global $API_params;

	$regex = '/
		(?: zend_parse_parameters(?:_throw)?               \s*\([^,]+
		|   zend_parse_(?:parameters_ex|method_parameters) \s*\([^,]+,[^,]+
		|   zend_parse_method_parameters_ex                \s*\([^,]+,[^,]+,[^,+]
		)
		,\s*"([^"]*)"\s*
		,\s*([^{;]*)
	/Sx';
	if (preg_match_all($regex, $txt, $matches, PREG_SET_ORDER | PREG_OFFSET_CAPTURE)) {

		$GLOBALS['current_function'] = $name;

		foreach ($matches as $m) {
			$GLOBALS['error_few_vars_given'] = false;
			update_lineno($offset + $m[2][1]);

			$vars = get_vars(substr($txt, 0, $m[0][1])); // limit var search to current location
			$params = get_params($vars, $m[2][0]);
			$optional = $varargs = false;
			$last_char = '';
			$j = -1;

			$spec = $m[1][0];
			$len = strlen($spec);
			for ($i = 0; $i < $len; ++$i) {
				$char = $spec[$i];
				switch ($char = $spec[$i]) {
					// separator for optional parameters
					case '|':
						if ($optional) {
							error("more than one optional separator at char #$i");
						} else {
							$optional = true;
							if ($i == $len-1) {
								error("unnecessary optional separator");
							}
						}
					break;

					// separate_zval_if_not_ref
					case '/':
						if (in_array($last_char, array('l', 'L', 'd', 'b'))) {
							error("the '/' specifier should not be applied to '$last_char'");
						}
					break;

					// nullable arguments
					case '!':
						if (in_array($last_char, array('l', 'L', 'd', 'b'))) {
							check_param($params, ++$j, 'zend_bool*', $optional);
						}
					break;

					// variadic arguments
					case '+':
					case '*':
						if ($varargs) {
							error("A varargs specifier can only be used once. repeated char at column $i");
						} else {
							check_param($params, ++$j, 'zval**', $optional);
							check_param($params, ++$j, 'int*', $optional);
							$varargs = true;
						}
					break;

					case 's':
					case 'p':
						check_param($params, ++$j, 'char**', $optional, $allow_uninit=true);
						check_param($params, ++$j, 'size_t*', $optional, $allow_uninit=true);
						if ($optional && !$params[$j-1][2] && !$params[$j][2]
								&& $params[$j-1][0] !== '**dummy**' && $params[$j][0] !== '**dummy**') {
							error("one of optional vars {$params[$j-1][0]} or {$params[$j][0]} must be initialized", 1);
						}
					break;

					case 'C':
						// C must always be initialized, independently of whether it's optional
						check_param($params, ++$j, 'zend_class_entry**', false);
					break;

					default:
						if (!isset($API_params[$char])) {
							error("unknown char ('$char') at column $i");
						}

						// If an is_null flag is in use, only that flag is required to be
						// initialized
						$allow_uninit = $i+1 < $len && $spec[$i+1] === '!'
								&& in_array($char, array('l', 'L', 'd', 'b'));

						foreach ($API_params[$char] as $exp) {
							check_param($params, ++$j, $exp, $optional, $allow_uninit);
						}
				}

				$last_char = $char;
			}
		}
	}
}


/** the main recursion function. splits files in functions and calls the other functions */
function recurse($path)
{
	foreach (scandir($path) as $file) {
		if ($file == '.' || $file == '..' || $file == 'CVS') continue;

		$file = "$path/$file";
		if (is_dir($file)) {
			recurse($file);
			continue;
		}

		// parse only .c and .cpp files
		if (substr_compare($file, '.c', -2) && substr_compare($file, '.cpp', -4)) continue;

		$txt = file_get_contents($file);
		// remove comments (but preserve the number of lines)
		$txt = preg_replace('@//.*@S', '', $txt);
		$txt = preg_replace_callback('@/\*.*\*/@SsU', function($matches) {
			return preg_replace("/[^\r\n]+/S", "", $matches[0]);
		}, $txt);

		$split = preg_split('/PHP_(?:NAMED_)?(?:FUNCTION|METHOD)\s*\((\w+(?:,\s*\w+)?)\)/S', $txt, -1, PREG_SPLIT_DELIM_CAPTURE | PREG_SPLIT_OFFSET_CAPTURE);

		if (count($split) < 2) continue; // no functions defined on this file
		array_shift($split); // the first part isn't relevant


		// generate the line offsets array
		$j = 0;
		$lines = preg_split("/(\r\n?|\n)/S", $txt, -1, PREG_SPLIT_DELIM_CAPTURE);
		$lines_offset = array();

		for ($i = 0; $i < count($lines); ++$i) {
			$j += strlen($lines[$i]) + strlen(@$lines[++$i]);
			$lines_offset[] = $j;
		}

		$GLOBALS['lines_offset'] = $lines_offset;
		$GLOBALS['current_file'] = $file;


		for ($i = 0; $i < count($split); $i+=2) {
			// if the /* }}} */ comment is found use it to reduce false positives
			// TODO: check the other indexes
			list($f) = preg_split('@/\*\s*}}}\s*\*/@S', $split[$i+1][0]);
			check_function(preg_replace('/\s*,\s*/S', '::', $split[$i][0]), $f, $split[$i][1]);
		}
	}
}

$dirs = array();

if (isset($argc) && $argc > 1) {
	if ($argv[1] == '-h' || $argv[1] == '-help' || $argv[1] == '--help') {
		echo <<<HELP
Synopsis:
    php check_parameters.php [directories]

HELP;
		exit(0);
	}
	for ($i = 1; $i < $argc; $i++) {
		$dirs[] = $argv[$i];
	}
} else {
	$dirs[] = PHPDIR;
}

foreach($dirs as $dir) {
	if (is_dir($dir)) {
		if (!is_readable($dir)) {
			echo "ERROR: directory '", $dir ,"' is not readable\n";
			exit(1);
		}
	} else {
		echo "ERROR: bogus directory '", $dir ,"'\n";
		exit(1);
	}
}

foreach ($dirs as $dir) {
	recurse(realpath($dir));
}
