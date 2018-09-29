<?php

class IncludeFiles extends ArrayIterator
{
	function __construct($path, $classes)
	{
		parent::__construct();
		foreach($classes as $c)
		{
			$this->append($path . '/' . strtolower($c) . '.inc');
		}
	}
}

$classes = array(
);

foreach (new IncludeFiles(dirname(__FILE__). '/..', $classes) as $file)
{
	require_once($file);
}

?>
