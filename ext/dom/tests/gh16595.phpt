--TEST--
GH-16595 (Another UAF in DOM -> cloneNode)
--EXTENSIONS--
dom
--CREDITS--
chibinz
--FILE--
<?php
$v0 = new DOMElement ( "jg" );
$v1 = new DOMDocument ( "Zb" );
$v2 = new DOMElement ( "IU" );
$v7 = new DOMElement ( "L" , null , "df" );
$v9 = new DOMDocument (  );

try { $v1 -> insertBefore ( $v0 , $v9 ); } catch (\Throwable) { }
$v0 -> replaceChildren ( $v7 );
$v7 -> before ( $v2 );
$v1 -> insertBefore ( $v0 );
$v2 -> cloneNode (  );
echo $v1->saveXML();
echo $v9->saveXML();
?>
--EXPECT--
<?xml version="Zb"?>
<jg xmlns:default="df"><IU/><default:L xmlns="df"/></jg>
<?xml version="1.0"?>
