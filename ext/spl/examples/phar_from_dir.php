<?php

/** @file   phar_from_dir.php
 * @brief   Create phar archive from directory
 * @ingroup examples
 * @author  Marcus Boerger
 * @date    2003 - 2007
 * @version 1.0
 *
 * Usage: php phar_create_from_dir.php \<archive\> \<directory\> [\<regex\>]
 *
 * Create phar archive \<archive\> using entries from \<directory\> that
 * optionally match \<regex\>.
 */

if ($argc < 3)
{
	echo <<<EOF
php phar_from_dir.php archive directory [regex]

Packs files in a given directory into a phar archive.

archive    name of the archive to create
directory  input directory to pack
regex      optional expression to match files in directory

EOF;
	exit(1);
}

$phar = new Phar($argv[1], 0, 'newphar');

$dir = new RecursiveDirectoryIterator($argv[2]);
$dir = new RecursiveIteratorIterator($dir);
if ($argc > 3)
{
	$dir = new RegexIterator($dir, '/'.$argv[3].'/');
}

$phar->begin();

foreach($dir as $file)
{
	echo "$file\n";
	copy($file, "phar://newphar/$file");
}

$phar->commit();

?>
