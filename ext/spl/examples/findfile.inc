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
		$list = split(';', $path);
		if (count($list) <= 1) {
			parent::__construct(new RecursiveIteratorIterator(new RecursiveDirectoryIterator($path)));
		} else {
			$it = new AppendIterator();
			foreach($list as $path) {
				$it->append(new RecursiveIteratorIterator(new RecursiveDirectoryIterator($path)));
			}
			parent::__construct($it);
		}
	}

	function accept()
	{
		return !strcmp($this->current(), $this->file);
	}
}

?>