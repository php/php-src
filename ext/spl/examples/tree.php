<?php

/** tree view example
 *
 * Usage: php tree.php <path>
 *
 * Simply specify the path to tree with parameter <path>.
 *
 * (c) Marcus Boerger, 2003
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

echo $argv[1]."\n";
foreach(new DirectoryGraphIterator($argv[1]) as $file) {
	echo $file . "\n";
}

?>