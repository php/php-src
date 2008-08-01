#!/usr/bin/php
<?php echo '<'.'?php';?>

/** @file phar.php
 * @ingroup Phar
 * @brief class Phar Pre Command
 * @author  Marcus Boerger
 * @date    2007 - 2008
 *
 * Phar Command
 */
foreach(array("SPL", "Reflection", "Phar") as $ext) {
	if (!extension_loaded($ext)) {
		echo "$argv[0] requires PHP extension $ext.\n";
		exit(1);
	}
}

<?php

$classes = array(
	'DirectoryTreeIterator',
	'DirectoryGraphIterator',
	'InvertedRegexIterator',
	'CLICommand',
	'PharCommand',
	);

foreach($classes as $name) {
	echo "if (!class_exists('$name', 0))\n{\n";
	$f = file(dirname(__FILE__) . '/phar/' . strtolower($name) . '.inc');
	unset($f[0]);
	$c = count($f);
	while ($c && (strlen($f[$c]) == 0 || $f[$c] == "\n" || $f[$c] == "\r\n")) {
		unset($f[$c--]);
	}
	if (substr($f[$c], -2) == "\r\n") {
		$f[$c] = substr($f[$c], 0, -2);
	}
	if (substr($f[$c], -1) == "\n") {
		$f[$c] = substr($f[$c], 0, -1);
	}
	if (substr($f[$c], -2) == '?>') {
		$f[$c] = substr($f[$c], 0,-2);
	}
	while ($c && (strlen($f[$c]) == 0 || $f[$c] == "\n" || $f[$c] == "\r\n")) {
		unset($f[$c--]);
	}
	echo join('', $f);
	echo "\n}\n\n";
}

echo 'new PharCommand($argc, $argv);'."\n";

?>
