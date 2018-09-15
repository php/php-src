<?php

/** @file autoload.inc
 * @ingroup Examples
 * @brief function __autoload
 * @author  Marcus Boerger
 * @date    2003 - 2005
 *
 * SPL - Standard PHP Library
 */

/** \internal
 * Tries to load class $classname from directory $dir.
 */
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

/**
 * @brief   Class loader for SPL example classes
 * @author  Marcus Boerger
 * @version 1.0
 *
 * Loads classes automatically from include_path as given by ini or from
 * current directory of script or include file.
 */
function __autoload($classname) {
	$classname = strtolower($classname);
	$inc = split(':', ini_get('include_path'));
	$inc[] = '.';
	$inc[] = dirname($_SERVER['PATH_TRANSLATED']);
	foreach($inc as $dir)
	{
		if (__load_class($classname, $dir))
		{
			fprintf(STDERR, 'Loading class('.$classname.")\n");
			return;
		}
	}
	fprintf(STDERR, 'Class not found ('.$classname.")\n");
}

?>
