<?php

class DirectoryTreeIterator extends RecursiveIteratorIterator
{
	function __construct($path)
	{
		parent::__construct(new CachingRecursiveIterator(new RecursiveDirectoryIterator($path)), 1);
	}
	
	function current()
	{
		$tree = '';
		for ($l=0; $l < $this->getLevel(); $l++) {
			$tree .= $this->getSubIterator($l)->hasNext() ? '| ' : '  ';
		}
		return $tree . ($this->getSubIterator($l)->hasNext() ? '|-' : '\-') 
		       . $this->getSubIterator($l);
	}
}

?>