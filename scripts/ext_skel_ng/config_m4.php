<?php

class config_m4 {
	# name, with[], libs[], language, files[]
	var $name;
	var $language = "c";
	var $with = false;
	var $libs = array();
	var $files = array();

	function __construct($name) {
		$this->name = $name;
	}

	function write_file() {
		$upname = strtoupper($this->name);
		
		// CVS ID header
		echo 
'dnl
dnl $ Id: $
dnl
';
				
	}
}

?>