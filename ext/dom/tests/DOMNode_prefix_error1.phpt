--TEST--
readonly DOMNode->prefix = 'bar'
--CREDITS--
Adam Martinson
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php
$Foo = new DOMElement('foo', NULL, "http://php.net/test");
try {
	$Foo->prefix = 'bar';
} catch (DOMException $Ex) {
	if ($Ex->getCode() == DOM_NO_MODIFICATION_ALLOWED_ERR) {
		echo "OK\n";
	} else {
		echo "$Ex\n";
	}
}
?>
--EXPECT--
OK
