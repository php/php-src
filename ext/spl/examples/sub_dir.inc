<?php

/**
 * @brief   Subdirectory aware directory iterator.
 * @author  Marcus Boerger
 * @version 1.0
 *
 * This directory iterator recursively returns all files and directories
 * within a given path.
 */
class sub_dir implements spl_sequence
{
	protected $adir = array();
	protected $cnt  = 0;
	protected $path = "";
	protected $curr = "";
	protected $nodots = true;

	/**
	 * Construct a directory from a path. 
	 *
	 * @param path    The path to iterate.
	 * @param nodots  Whether or not to display the entries '.' and '..'.
	 */
	function __construct($path, $nodots = true) {
		$this->cnt = 0;
		$this->path = $path;
	}
	
	/**
	 * Rewind the directory.
	 *
	 * @return void
	 */
	function rewind() {
	    while($this->cnt) {
	    	unset($this->adir[$this->cnt--]);
	    }
		$dir = new spl_dir($this->path);
		$dir->path = "";
		$this->adir[1] = $dir;
		$this->cnt = 1;
		if ($this->nodots) {
			while ($this->has_more()) {
				$ent = $this->current();
				if ($ent != '.' && $ent != '..') {
					break;
				}
				$this->next();
			}
		}
	}

	/**
	 * Move to net dir or file entry.
	 *
	 * @return void
	 */
	function next() {
		if ($this->cnt) {
			$dir = $this->adir[$this->cnt];
			$ent = $dir->current();
			$path = $dir->get_path().'/'.$ent;
			if ($ent != '.' && $ent != '..' && is_dir($path)) {
				$new = new spl_dir($path);
				$new->path = $dir->path.$ent.'/';
				$new->cnt = $this->cnt++;
				$this->adir[$this->cnt] = $new;
				if ($this->nodots) {
					while ($new->has_more()) {
						$ent = $new->current();
						if ($ent != '.' && $ent != '..') {
							break;
						}
						$new->next();
					}
				}
			}
			$dir->next();
		}
	}

	/**
	 * @return Whether more dirs or files entries are available.
	 */
	function has_more() {
		while ($this->cnt) {
			$dir = $this->adir[$this->cnt];
			if ($dir->has_more()) {
				return true;
			}
			unset($this->adir[$this->cnt--]);
		}
		return false;
	}

	/**
	 * @return The current dir or file entry.
	 */
	function current() {
		if ($this->cnt) {
			$dir = $this->adir[$this->cnt];
			return $dir->path . $dir->current();
		}
		throw new exception("No more elements available");
	}

	/**
	 * Hidden __clone
	 */
	protected function __clone() {
		// disallow clone 
	}
}

?>