<?php

function __load_class($classname, $dir)
{
	$file = $dir . '/' . $classname . '.inc';
	if (file_exists($file))
	{
		require_once($file);
		return true;
	}
	return false;
}

function __autoload($classname) {
	$classname = strtolower($classname);
	foreach(split(':', ini_get('include_path')) as $dir)
	{
		if (__load_class($classname, $dir))
		{
			fprintf(STDERR, 'Loading class('.$classname.")\n");
			return;
		}
	}
	if (!__load_class($classname, '.'))
	if (!__load_class($classname, dirname($_SERVER['PATH_TRANSLATED'])))
	fprintf(STDERR, 'Class not found ('.$classname.")\n");
	return;
}

?>