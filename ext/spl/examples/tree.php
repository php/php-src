<?php

/* tree view example
 *
 * Usage: php tree.php <path>
 *
 * Simply specify the path to tree with parameter <path>.
 *
 * (c) Marcus Boerger
 */

require_once("sub_dir.inc");

foreach(new sub_dir($argv[1]) as $f) {
	echo "$f\n";
}

?>