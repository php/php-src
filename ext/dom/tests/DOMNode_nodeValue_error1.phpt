--TEST--
readonly DOMNode->nodeValue = 'bar'
--CREDITS--
Adam Martinson
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php
$Foo = new DOMElement('foo');
try {
	$Foo->nodeValue = 'bar';
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
