<?php

/* tree view example
 *
 * Usage: php filter_tree.php <path> <regex>
 *
 * Simply specify the path to tree with parameter <path>.
 * The regular expression <regex> is used to filter the tree.
 *
 * (c) Marcus Boerger
 */

require_once("sub_dir.inc");
require_once("filter.inc");

foreach(new filter(new sub_dir($argv[1]), $argv[2]) as $f) {
	echo "$f\n";
}

?>