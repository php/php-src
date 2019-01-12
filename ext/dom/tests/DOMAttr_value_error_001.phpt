--TEST--
readonly DOMAttr->value = 'bar'
--CREDITS--
Adam Martinson
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php
$Attr = new DOMAttr('foo');
try {
	$Attr->value = 'bar';
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
