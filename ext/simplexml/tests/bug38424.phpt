--TEST--
Bug #38424 (Different attribute assignment if new or exists)
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php

$xml = simplexml_load_string('<xml></xml>');

$str = "abc & def" ;

$xml["a1"] = "" ;
$xml["a1"] = htmlspecialchars($str,ENT_NOQUOTES) ;

$xml["a2"] = htmlspecialchars($str,ENT_NOQUOTES) ;

$xml["a3"] = "" ;
$xml["a3"] = $str ;

$xml["a4"] = $str ;

echo $xml->asXML();
?>
--EXPECT--	
<?xml version="1.0"?>
<xml a1="abc &amp;amp; def" a2="abc &amp;amp; def" a3="abc &amp; def" a4="abc &amp; def"/>
