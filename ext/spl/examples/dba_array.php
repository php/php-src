<?php

/* dba array utility
 *
 * Usage php dba_dump <file> <handler> <key> [<value>]
 *
 * If <value> is specified then <key> is set to <value> in <file>.
 * Else the value of <key> is printed only.
 *
 * Note: configure with --enable-dba 
 *
 * (c) Marcus Boerger
 */

class DbaArray implements ArrayAccess {
	private $db;

	function __construct($file, $handler)
	{
		$this->db = dba_popen($file, "c", $handler);
		if (!$this->db) {
			throw new exception("Databse could not be opened");
		}
	}

	function __destruct()
	{
		dba_close($this->db);
	}

	function get($name)
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

	function set($name, $value)
	{
		//dba_replace($name, serialize($value), $this->db);
		dba_replace($name, $value, $this->db);
		return $value;
	}

	function exists($name)
	{
		return dba_exists($name, $this->db);
	}

	function del($name)
	{
		return dba_delete($name, $this->db);
	}
}

try {
	if ($argc > 2) {
		$dba = new DbaArray($argv[1], $argv[2]);
		if ($dba && $argc > 3) {
			if ($argc > 4) {
				$dba[$argv[3]] = $argv[4];
			}
			var_dump(array('Index' => $argv[3], 'Value' => $dba[$argv[3]]));
		}
		unset($dba);
	}
	else
	{
		echo "Not enough parameters\n";
		exit(1);
	}
}
catch (exception $err) {
	var_dump($err);
	exit(1);
}
?>