<?php

/**
 * @brief   This implements an dba iterator.
 * @author  Marcus Boerger
 * @version 1.0
 */
class dba_reader implements spl_sequence_assoc
{

	private $db = NULL;
	private $key = false;
	private $val = false;

	/**
	 * Open database $file with $handler in read only mode.
	 *
	 * @param file    Database file to open.
	 * @param handler Handler to use for database access.
	 */
	function __construct($file, $handler) {
		$this->db = dba_open($file, 'r', $handler);
	}
	
	/**
	 * Close database.
	 */
	function __destruct() {
		if ($this->db) {
			dba_close($this->db);
		}
	}

	/**
	 * Rewind to first element.
	 */
	function rewind() {
		if ($this->db) {
			$this->key = dba_firstkey($this->db);
		}
	}

	/**
	 * @return Current data.
	 */
	function current() {
		return $this->val;
	}

	/**
	 * Move to next element.
	 *
	 * @return void
	 */
	function next() {
		if ($this->db) {
			$this->key = dba_nextkey($this->db);
			if ($this->key !== false) {
				$this->val = dba_fetch($this->key, $this->db);
			}
		}
	}

	/**
	 * @return Whether more elements are available.
	 */
	function hasMore() {
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