--TEST--
DOMDocument::loadHTML
--CREDITS--
Frank Cassedanne franck@ouarz.net
#London TestFest 2008
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadHTML("<html><body><p>Test<br></p></body></html>");
echo $doc->saveHTML();
?>
--EXPECTF--
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" "http://www.w3.org/TR/REC-html40/loose.dtd">
<html><body><p>Test<br></p></body></html>
