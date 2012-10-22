<?php

/** @file dbareader.inc
 * @ingroup Examples
 * @brief class DbaReader
 * @author  Marcus Boerger
 * @date    2003 - 2005
 *
 * SPL - Standard PHP Library
 */

/** @ingroup Examples
 * @brief   This implements a DBA Iterator.
 * @author  Marcus Boerger
 * @version 1.0
 */
class DbaReader implements Iterator
{

	protected $db = NULL;
	private $key = false;
	private $val = false;

	/**
	 * Open database $file with $handler in read only mode.
	 *
	 * @param file    Database file to open.
	 * @param handler Handler to use for database access.
	 */
	function __construct($file, $handler) {
		if (!$this->db = dba_open($file, 'r', $handler)) {
		    throw new exception('Could not open file ' . $file);
		}
	}
	
	/**
	 * Close database.
	 */
	function __destruct() {
		dba_close($this->db);
	}

	/**
	 * Rewind to first element.
	 */
	function rewind() {
		$this->key = dba_firstkey($this->db);
		$this->fetch_data();
	}

	/**
	 * Move to next element.
	 *
	 * @return void
	 */
	function next() {
		$this->key = dba_nextkey($this->db);
		$this->fetch_data();
	}

    /**
     * Fetches the current data if $key is valid
     */	
	private function fetch_data() {
		if ($this->key !== false) {
			$this->val = dba_fetch($this->key, $this->db);
		}
	}

	/**
	 * @return Current data.
	 */
	function current() {
		return $this->val;
	}

	/**
	 * @return Whether more elements are available.
	 */
	function valid() {
		if ($this->db && $this->key !== false) {
			return true;
		} else {
			return false;
		}
	}

	/**
	 * @return Current key.
	 */
	function key() {
		return $this->key;
	}
}

?>