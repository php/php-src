<?php

class DirectoryTree extends RecursiveIteratorIterator
{
	function __construct($path) {
		parent::__construct(new DirectoryFilterDots($path));
	}
}

?>