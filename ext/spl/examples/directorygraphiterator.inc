<?php

class DirectoryGraphIterator extends DirectoryTreeIterator
{
	function __construct($path)
	{
		RecursiveIteratorIterator::__construct(new CachingRecursiveIterator(new ParentIterator(new RecursiveDirectoryIterator($path)), CIT_CALL_TOSTRING|CIT_CATCH_GET_CHILD), 1);
	}
}

?>