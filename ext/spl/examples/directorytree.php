<?php

/** tree view example
 *
 * Usage: php DirectoryTree.php <path>
 *
 * Simply specify the path to tree with parameter <path>.
 *
 * (c) Marcus Boerger
 */

$length = $argc > 3 ? $argv[3] : NULL;

foreach(new LimitIterator(new DirectoryTreeIterator($argv[1]), @$argv[2], $length) as $pathname => $file) {
	echo "$file\n";
}

?>