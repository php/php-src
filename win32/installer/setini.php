<?php
/* $Id$
 * Wez Furlong <wez@thebrainroom.com>
 *
 * Set options in a php.ini file.
 * This is a support script for the installer.
 *
 * $argv[1] is the path to the ini file
 * $argv[2] is the path to the option file
 */

$ini_name = $argv[1];
$option_file = $argv[2];

$options = explode("\n", file_get_contents($option_file));
$opts = array();

/* Parse the options */
foreach ($options as $line) {
	list($name, $value) = explode("=", $line);

	if ($name == "extension") {
		$pat = "/^;?extension\s*=\s*" . $value . "/i";
	} else {
		$pat = "/^;?$name\s*=\s*/i";
	}

	$opts[] = array('pat' => $pat, 'name' => $name, 'value' => $value);
}

$new_name = $ini_name . "~";
$dest = fopen($new_name, "w");

if (!$dest) {
 	die("Cannot create temporary file!");
}

$lines = file($ini_name);

foreach ($lines as $line) {

	foreach ($opts as $k => $optdata) {
		extract($optdata);
		
		if (preg_match($pat, $line)) {
			echo "Found: $line\n";
			$line = "$name=$value\r\n";
			echo" New value: $line\n";

			// No need to match again
			unset($opts[$k]);
			break;
		}
	}

	fwrite($dest, $line);
}

fclose($dest);

unlink($ini_name);
rename($new_name, $ini_name);

?>
