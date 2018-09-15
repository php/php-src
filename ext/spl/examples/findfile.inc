<?php

/** @file findfile.inc
 * @ingroup Examples
 * @brief class FindFile
 * @author  Marcus Boerger
 * @date    2003 - 2005
 *
 * SPL - Standard PHP Library
 */

if (!class_exists("FindFile", false)) require_once("findfile.inc");
if (!class_exists("AppendIterator", false)) require_once("appenditerator.inc");

/** @ingroup Examples
 * @brief   Base class to find files
 * @author  Marcus Boerger
 * @version 1.1
 *
 */
class FindFile extends FilterIterator
{
	/** @internal filename to find */
	private $file;

	/** Construct from path and filename
	 *
	 * @param $path the directory to search in
	 *              If path contains ';' then this parameter is split and every
	 *              part of it is used as separate directory.
	 * @param $file the name of the files to search fro
	 */
	function __construct($path, $file)
	{
		$this->file = $file;
		$list = split(PATH_SEPARATOR, $path);
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

	/** @return whether the current file matches the given filename
	 */
	function accept()
	{
		return !strcmp($this->current(), $this->file);
	}

	/** @return the filename to search for.
	 * @note This may be overloaded and contain a regular expression for an
	 *       extended class that uses regular expressions to search.
	 */
	function getSearch()
	{
		return $this->file;
	}
}

?>
