--TEST--
Bug #67474 getElementsByTagNameNS and default namespace
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php
declare(strict_types=1);

$doc = new DOMDocument();
$doc->loadXML('<root xmlns:x="x"><a/><x:a/></root>');
$list = $doc->getElementsByTagNameNS('', 'a');
var_dump($list->length);
$list = $doc->getElementsByTagNameNS(null, 'a');
var_dump($list->length);

$elem = $doc->documentElement;
$list = $elem->getElementsByTagNameNS('', 'a');
var_dump($list->length);
$list = $elem->getElementsByTagNameNS(null, 'a');
var_dump($list->length);

?>
--EXPECT--
int(1)
int(1)
int(1)
int(1)
