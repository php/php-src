<?php

/** tree view example
 *
 * Usage: php Tree.php <path>
 *
 * Simply specify the path to tree with parameter <path>.
 *
 * (c) Marcus Boerger
 */

foreach(new DirectoryGraphIterator($argv[1]) as $file) {
	echo $file . "\n";
}

?>