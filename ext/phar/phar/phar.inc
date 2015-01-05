<?php

/**
 * @file phar.inc
 * @ingroup Phar
 * @brief class Phar
 * @author  Marcus Boerger
 * @date    2007 - 2008
 *
 * Phar Command
 */
// {{{ class Phar extends PHP_Archive
/**
 * Phar class
 * 
 * @ingroup Phar
 * @brief   Phar implementation
 * @author  Marcus Boerger
 * @version 1.0
 */
class Phar extends PHP_Archive implements RecursiveIterator
{
	function getSignature()
	{
		return false;
	}

	function getAlias()
	{
		return false;
	}

	function rewind()
	{
	}

	function valid()
	{
		return false;
	}

	function current()
	{
	}

	function key()
	{
	}

	function next()
	{
	}

	function hasChildren()
	{
		return false;
	}

	function getChildren()
	{
	}

	function hasMetadata()
	{
	}

	function getMetadata()
	{
	}

	function getStub()
	{
	}

	function setStub()
	{
	}
}

?>