<?php

/** @file   directorytree.php
 * @brief   Program Directory tree example
 * @ingroup Examples
 * @author  Marcus Boerger
 * @date    2003 - 2005
 *
 * Usage: php directorytree.php \<path\> [\<start\> [\<count\>]]
 *
 * Simply specify the path to tree with parameter \<path\>.
 */

if ($argc < 2) {
	echo <<<EOF
Usage: php ${_SERVER['PHP_SELF']} <path>

Displays a graphical directory tree for the given <path>.

<path> The directory for which to generate the directory tree graph.


EOF;
	exit(1);
}

if (!class_exists("DirectoryTreeIterator", false)) require_once("directorytreeiterator.inc");

$length = $argc > 3 ? $argv[3] : -1;

echo $argv[1]."\n";
foreach(new LimitIterator(new DirectoryTreeIterator($argv[1]), @$argv[2], $length) as $key=>$file) {
//foreach(new DirectoryTreeIterator($argv[1]) as $file) {
	echo $file . "\n";
}

?>
