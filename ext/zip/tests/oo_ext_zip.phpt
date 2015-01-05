--TEST--
Extending Zip class and array property
--SKIPIF--
<?php
/* $Id$ */
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
class myZip extends ZipArchive {
	private $test = 0;
	public $testp = 1;
	private $testarray = array();

	public function __construct() {
		$this->testarray[] = 1;
		var_dump($this->testarray);
	}
}

$z = new myZip;
?>
--EXPECTF--
array(1) {
  [0]=>
  int(1)
}
