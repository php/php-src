<?php

/** Find a specific file by name.
 *
 * Usage: php findfile.php <path> <name>
 *
 * <path>  Path to search in.
 * <name>  Filename to look for.
 *
 * (c) Marcus Boerger, 2003
 */

if ($argc < 3) {
	echo <<<EOF
Usage: php findfile.php <file> <name>

Find a specific file by name.

<path>  Path to search in.
<name>  Filename to look for.


EOF;
	exit(1);
}

foreach(new FindFile($argv[1], $argv[2]) as $file) echo $file->getPathname()."\n";
?>