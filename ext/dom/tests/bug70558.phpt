--TEST--
Bug #70558 ("Couldn't fetch" error in DOMDocument::registerNodeClass())
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php

class X extends \DOMDocument {

	public function __clone() {
		var_dump($this->registerNodeClass('DOMDocument', 'X'));
	}
}

$dom = clone (new X());
?>
--EXPECT--
bool(true)
