<?php

abstract class license 
{
	function __construct($options = array()) {
		$this->options = $options;
	}

	static function factory($name, $options=array()) {
		$classname = "license_".strtolower($name);

		if (!class_exists($classname)) {
			if (file_exists("./$classname.php")) {
				require_once "./$classname.php";
			}
		}

		return 
			class_exists($classname) 
			? new $classname($options) 
			: false;
	}

	abstract function license_comment();

	function write_license_file($path = "./LICENSE") {
		$fp = fopen($path, "w");

		if (is_resource($fp)) {
			fputs($fp, $this->license_file_text()); 
			fclose($fp);
			return true;
		}

		return false;
	}
	
	abstract function license_file_text();
}


?>