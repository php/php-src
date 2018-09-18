<?php

/** @file   findfile.php
 * @brief   Program Find a specific file by name.
 * @ingroup Examples
 * @author  Marcus Boerger
 * @date    2003 - 2005
 *
 * Usage: php findfile.php \<path\> \<name\>
 *
 * \<path\>  Path to search in. You can specify multiple paths by separating
 *         them with ';'.
 * \<name\>  Filename to look for.
 */

if ($argc < 3) {
	echo <<<EOF
Usage: php findfile.php <path> <name>

Find a specific file by name.

<path>  Path to search in.
<name>  Filename to look for.


EOF;
	exit(1);
}

if (!class_exists("FindFile", false)) require_once("findfile.inc");

foreach(new FindFile($argv[1], $argv[2]) as $file) echo $file->getPathname()."\n";
?>
