<?php

/**
 * @brief   Base class to find files
 * @author  Marcus Boerger
 * @version 1.0
 *
 */
class FindFile extends FilterIterator
{
	protected $file;

	function __construct($path, $file)
	{
		$this->file = $file;
		parent::__construct(new RecursiveIteratorIterator(new RecursiveDirectoryIterator($path)));
	}

	function accept()
	{
		return !strcmp($this->current(), $this->file);
	}
}

?>