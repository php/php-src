<?php

/** @file   tree.php
 * @brief   Program Tree view example
 * @ingroup Examples
 * @author  Marcus Boerger
 * @date    2003 - 2004
 *
 * Usage: php tree.php \<path\>
 *
 * Simply specify the path to tree with parameter \<path\>.
 */

// The following line only operates on classes which are converted to c already.
// But does not generate a graphical output.
//foreach(new RecursiveIteratorIterator(new ParentIterator(new RecursiveDirectoryIterator($argv[1])), 1) as $file) {

if ($argc < 2) {
	echo <<<EOF
Usage: php ${_SERVER['PHP_SELF']} <path>

Displays a graphical tree for the given <path>.

<path> The directory for which to generate the tree graph.


EOF;
	exit(1);
}

if (!class_exists("DirectoryTreeIterator")) require_once("directorytreeiterator.inc");

echo $argv[1]."\n";
foreach(new DirectoryTreeIterator($argv[1]) as $file)
{
	echo $file . "\n";
}

?>