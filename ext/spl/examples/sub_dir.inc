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
	protected $path = '';
	protected $curr = '';
	protected $nodots = true;

	/**
	 * Construct a directory from a path. 
	 *
	 * @param path    The path to iterate.
	 * @param nodots  Whether or not to display the entries '.' and '..'.
	 */
	function __construct($path, $nodots = true, $graph = false) {
		$this->cnt = 0;
		$this->path = $path;
		$this->nodots = $nodots;
		$this->graph = $graph;
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
			while ($dir->hasMore()) {
				$ent = $dir->current();
				if ($ent != '.' && $ent != '..') {
					break;
				}
				$dir->next();
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
			$path = $dir->getPath().'/'.$ent;
			if ($ent != '.' && $ent != '..' && is_dir($path)) {
				$new = new spl_dir($path);
				$new->path = $dir->path.$ent.'/';
				$new->cnt = $this->cnt++;
				$this->adir[$this->cnt] = $new;
				if ($this->nodots) {
					$dir->has_more = false;
					while ($new->hasMore()) {
						$ent = $new->current();
						if ($ent != '.' && $ent != '..') {
							$dir->has_more = true;
							break;
						}
						$new->next();
					}
				} else {
					$dir->has_more = $dir->hasMore();
				}
			}
			$dir->next();
		}
	}

	/**
	 * @return Whether more dirs or files entries are available.
	 */
	function hasMore() {
		while ($this->cnt) {
			$dir = $this->adir[$this->cnt];
			if ($dir->hasMore()) {
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
			if ($this->graph) {
				$prefix = '';
				for ($i = 1; $i < $this->cnt; $i++) {
					$dir = $this->adir[$i];
					$prefix .= $dir->hasMore() ? '| ' : '  ';
				}
				$dir = $this->adir[$this->cnt];
				$ent = $dir->current();
				$prefix .= $dir->hasMore() ? '+-' : '\-';
				return $prefix . $ent;
			} else {
				$dir = $this->adir[$this->cnt];
				return $dir->path . $dir->current();
			}
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