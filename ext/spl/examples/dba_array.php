<?php

/** @file   dba_array.php
 * @brief   Program DBA array utility
 * @ingroup Examples
 * @author  Marcus Boerger
 * @date    2003 - 2005
 *
 * Usage php dba_array.php \<file\> \<handler\> \<key\> [\<value\>]
 *
 * If \<value\> is specified then \<key\> is set to \<value\> in \<file\>.
 * Else the value of \<key\> is printed only.
 *
 * Note: configure with --enable-dba 
 */

if ($argc < 4) {
	echo <<<EOF
Usage: php ${_SERVER['PHP_SELF']} <file> <handler> <key> [<value>]

If <value> is specified then <key> is set to <value> in <file>.
Else the value of <key> is printed only.


EOF;
	exit(1);
}

if (!class_exists("DbaReader", false)) require_once("dbareader.inc");

try {
	if ($argc > 2) {
		$dba = new DbaArray($argv[1], $argv[2]);
		if ($dba && $argc > 3) {
			if ($argc > 4) {
				$dba[$argv[3]] = $argv[4];
			}
			var_dump(array('Index' => $argv[3], 'Value' => $dba[$argv[3]]));
		}
		unset($dba);
	}
	else
	{
		echo "Not enough parameters\n";
		exit(1);
	}
}
catch (exception $err) {
	var_dump($err);
	exit(1);
}
?>