--TEST--
GHSA-x8r7-mg3g-74hg (Lifetime issue in Dom\XMLDocument serialiser)
--EXTENSIONS--
dom
--CREDITS--
EliseZeroTwo
--FILE--
<?php

$doc = Dom\HTMLDocument::createFromString('<!DOCTYPE html><svg xmlns:xlink="urn:x" xlink:href="a"><g xmlns:xlink="http://www.w3.org/1999/xlink"/></svg><svg/>', LIBXML_NOERROR);
echo $doc->saveXml(), "\n";

$doc = Dom\XMLDocument::createFromString('<cat><kitten xmlns:cat="meow"/></cat>');
$doc->documentElement->setAttributeNS('meow', 'miau', 'miaou');
echo $doc->saveXml(), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml"><head></head><body><svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="urn:x" xmlns:ns1="http://www.w3.org/1999/xlink" ns1:href="a"><g xmlns:xlink="http://www.w3.org/1999/xlink"/></svg><svg xmlns="http://www.w3.org/2000/svg"/></body></html>
<?xml version="1.0" encoding="UTF-8"?>
<cat xmlns:ns1="meow" ns1:miau="miaou"><kitten xmlns:cat="meow"/></cat>
