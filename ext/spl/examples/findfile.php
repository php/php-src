<?php

class FindFile extends SearchIterator
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