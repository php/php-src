<?php

/* tree view example
 *
 * Usage: php tree.php <path>
 *
 * Simply specify the path to tree with parameter <path>.
 *
 * (c) Marcus Boerger
 */

class sub_dir implements spl_sequence
{
	protected $adir = array();
	protected $cnt  = 0;
	protected $path = "";
	protected $curr = "";
	protected $nodots = true;

	function __construct($path, $nodots = true) {
		$this->cnt = 0;
		$this->path = $path;
	}
	
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

	function current() {
		if ($this->cnt) {
			$dir = $this->adir[$this->cnt];
			return $dir->path . $dir->current();
		}
		throw new exception("No more elements available");
	}
}

foreach(new sub_dir($argv[1]) as $f) {
	echo "$f\n";
}

?>