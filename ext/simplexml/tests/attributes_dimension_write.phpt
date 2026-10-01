--TEST--
Creating new attributes via dimension and property writes on attributes()
--FILE--
<?php
$x = simplexml_load_string('<r a="1"/>');
$x->attributes()['new'] = 'v';
echo $x->asXML();

$a = simplexml_load_string('<r/>');
$a->attributes()['created'] = 'yes';
echo $a->asXML();

$b = simplexml_load_string('<r a="1"/>');
$attrs = $b->attributes();
$attrs->other = 2;
echo $b->asXML();

$c = simplexml_load_string('<r a="1"/>');
$c->attributes()['a'] = '2';
echo $c->asXML();
?>
--EXPECT--
<?xml version="1.0"?>
<r a="1" new="v"/>
<?xml version="1.0"?>
<r created="yes"/>
<?xml version="1.0"?>
<r a="1" other="2"/>
<?xml version="1.0"?>
<r a="2"/>
