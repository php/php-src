--TEST--
Test DOMDocument::createAttribute() for expected expection thrown when wrong parameter passed
--FILE--
<?php
$dom = new DOMDocument();

try {
	$attr = $dom->createAttribute(0);
}
catch(DOMException $e) {
	$code = $e->getCode();
	if(DOM_INVALID_CHARACTER_ERR === $code) {
		echo "PASS";
	}
	else {
		echo 'Wrong exception code';
	}
}
catch(Exception $e) {
	echo 'Wrong exception thrown';
}

?>
--EXPECTF--
PASS
