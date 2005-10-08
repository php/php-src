<?php

/** @file   directorytree_size.php
 * @brief   Program Directory tree size example
 * @ingroup Examples
 * @author  Marcus Boerger
 * @date    2003 - 2005
 *
 * Usage: php directorytree_size.php \<path\>
 *
 * Simply specify the path to tree with parameter \<path\>.
 */

if ($argc < 2) {
	echo <<<EOF
Usage: php ${_SERVER['PHP_SELF']} <path>

Displays a graphical directory tree for the given <path> with size info for all 
files.

<path> The directory for which to generate the directory tree graph.


EOF;
	exit(1);
}

if (!class_exists("RecursiveTreeIterator", false)) require_once("recursivetreeiterator.inc");

echo $argv[1]."\n";
foreach(new RecursiveTreeIterator(new RecursiveDirectoryIterator($argv[1], RecursiveDirectoryIterator::NEW_CURRENT_AND_KEY), RecursiveTreeIterator::BYPASS_CURRENT) as $file => $fileinfo) {
	echo $file . " (" . $fileinfo->getSize() . ")\n";
}

?>