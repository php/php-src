<?php

/** @file regexfindfile.inc
 * @ingroup Examples
 * @brief class RegexFindFile
 * @author  Marcus Boerger
 * @date    2003 - 2005
 *
 * SPL - Standard PHP Library
 */

/** @ingroup Examples
 * @brief   Find files by regular expression
 * @author  Marcus Boerger
 * @version 1.1
 *
 */
class RegexFindFile extends FindFile
{
	/** Construct from path and regular expression
	 *
	 * @param $path the directory to search in
	 *              If path contains ';' then this parameter is split and every
	 *              part of it is used as separate directory.
	 * @param $regex perl style regular expression to find files with
	 */
	function __construct($path, $regex)
	{
		parent::__construct($path, $regex);
	}

	/** @return whether the current filename matches the regular expression.
	 */
	function accept()
	{
		return preg_match($this->getSearch(), $this->current());
	}
}

?>