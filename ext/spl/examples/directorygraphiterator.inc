<?php

class DirectoryGraphIterator extends DirectoryTreeIterator
{
	function __construct($path)
	{
		RecursiveIteratorIterator::__construct(new CachingRecursiveIterator(new ParentIterator(new RecursiveDirectoryIterator($path)), true, true), 1);
	}
}

?>