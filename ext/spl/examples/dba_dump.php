<?php

/* dba dump utility
 *
 * Usage php dba_dump <file> <handler>
 *
 * Note: configure with --enable-dba 
 *
 * (c) Marcus Boerger
 */

class dba_reader implements spl_sequence_assoc
{

	private $db = NULL;
	private $key = false;
	private $val = false;

	function __construct($file, $handler) {
		$this->db = dba_open($file, 'r', $handler);
	}
	
	function __destruct() {
		if ($this->db) {
			dba_close($this->db);
		}
	}

	function rewind() {
		if ($this->db) {
			$this->key = dba_firstkey($this->db);
		}
	}

	function current() {
		return $this->val;
	}

	function next() {
		if ($this->db) {
			$this->key = dba_nextkey($this->db);
			if ($this->key !== false) {
				$this->val = dba_fetch($this->key, $this->db);
			}
		}
	}

	function has_more() {
		if ($this->db && $this->key !== false) {
			return true;
		} else {
			return false;
		}
	}

	function key() {
		return $this->key;
	}
}

$db = new dba_reader($argv[1], $argv[2]);
foreach($db as $key => $val) {
	echo "'$key' => '$val'\n";
}

?>