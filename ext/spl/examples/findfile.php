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

class FindFile extends FilterIterator
{
	protected $file;

	function __construct($path, $file) {
		$this->file = $file;
		parent::__construct(new RecursiveIteratorIterator(new RecursiveDirectoryIterator($path)));
	}
	function accept() {
		return !strcmp($this->it->current(), $this->file);
	}
}

foreach(new FindFile($argv[1], $argv[2]) as $pathname => $file) echo "$pathname\n";
?>