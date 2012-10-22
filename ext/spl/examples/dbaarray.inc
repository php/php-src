<?php

/** @file dbaarray.inc
 * @ingroup Examples
 * @brief class DbaArray
 * @author  Marcus Boerger
 * @date    2003 - 2005
 *
 * SPL - Standard PHP Library
 */

if (!class_exists("DbaReader", false)) require_once("dbareader.inc");

/** @ingroup Examples
 * @brief   This implements a DBA Array
 * @author  Marcus Boerger
 * @version 1.0
 */
class DbaArray extends DbaReader implements ArrayAccess
{

	/**
	 * Open database $file with $handler in read only mode.
	 *
	 * @param file    Database file to open.
	 * @param handler Handler to use for database access.
	 */
	function __construct($file, $handler)
	{
		$this->db = dba_popen($file, "c", $handler);
		if (!$this->db) {
			throw new exception("Databse could not be opened");
		}
	}

	/**
	 * Close database.
	 */
	function __destruct()
	{
		parent::__destruct();
	}

	/**
	 * Read an entry.
	 *
	 * @param $name key to read from
	 * @return value associated with $name
	 */
	function offsetGet($name)
	{
		$data = dba_fetch($name, $this->db); 
		if($data) {
			if (ini_get('magic_quotes_runtime')) {
				$data = stripslashes($data);
			}
			//return unserialize($data);
			return $data;
		}
		else 
		{
			return NULL;
		}
	}

	/**
	 * Set an entry.
	 *
	 * @param $name key to write to
	 * @param $value value to write
	 */
	function offsetSet($name, $value)
	{
		//dba_replace($name, serialize($value), $this->db);
		dba_replace($name, $value, $this->db);
		return $value;
	}

	/**
	 * @return whether key $name exists.
	 */
	function offsetExists($name)
	{
		return dba_exists($name, $this->db);
	}

	/**
	 * Delete a key/value pair.
	 *
	 * @param $name key to delete.
	 */
	function offsetUnset($name)
	{
		return dba_delete($name, $this->db);
	}
}

?>