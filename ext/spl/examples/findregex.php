<?php

/** @file   findregex.php
 * @brief   Program Find a specific file by name.
 * @ingroup Examples
 * @author  Marcus Boerger, Adam Trachtenberg
 * @date    2004
 *
 * Usage: php findregex.php \<path\> \<name\>
 *
 * \<path\>  Path to search in.
 * \<name\>  Filename to look for.
 */

if ($argc < 3) {
	echo <<<EOF
Usage: php findregex.php <file> <name>

Find a specific file by name.

<path>  Path to search in.
<name>  Regex for filenames to look for.


EOF;
	exit(1);
}

if (!class_exists("RegexFindFile", false)) require_once("regexfindfile.inc");

foreach(new RegexFindFile($argv[1], $argv[2]) as $file)
{
	echo $file->getPathname()."\n";
}

?>
