--TEST--
var_export() and classes
--FILE--
<?php
class kake {
	public $mann;
	protected $kvinne;

	function __construct()
	{
		$this->mann = 42;
		$this->kvinne = 43;
	}
}

$kake = new kake;

var_export($kake);
?>
--EXPECT--
kake::__set_state(array(
   'mann' => 42,
   'kvinne' => 43,
))
