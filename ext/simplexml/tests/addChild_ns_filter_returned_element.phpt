--TEST--
SimpleXMLElement::addChild() wrong namespace filter on returned element
--EXTENSIONS--
simplexml
--FILE--
<?php
$x = new SimpleXMLElement('<r xmlns:a="http://example.com"/>');
$c = $x->addChild('a:kid', null, 'http://example.com');
$c->addChild('inner', 'v');
echo trim($x->asXML()), "\n";
echo (string) $c->inner, "\n";
var_dump(isset($c->inner));

$y = new SimpleXMLElement('<r xmlns:a="http://example.com"/>');
$d = $y->addChild('kid', null, 'http://example.com');
$d->addChild('inner', 'w');
echo trim($y->asXML()), "\n";
echo (string) $d->inner, "\n";

$z = new SimpleXMLElement('<r/>');
$e = $z->addChild('a:kid');
$e->addChild('inner', 'z');
echo trim($z->asXML()), "\n";
echo (string) $e->inner, "\n";
var_dump(isset($e->inner));

$q = new SimpleXMLElement('<p:r xmlns:p="http://example.com/p"/>');
$f = $q->addChild('kid');
$f->addAttribute('id', '7');
echo trim($q->asXML()), "\n";
echo (string) $f['id'], "\n";

$m = new SimpleXMLElement('<r xmlns:a="http://example.com"/>');
$g = $m->addChild('kid', null, 'http://example.com');
$g->addAttribute('id', '8');
echo trim($m->asXML()), "\n";
var_dump(isset($g['id']));
?>
--EXPECT--
<?xml version="1.0"?>
<r xmlns:a="http://example.com"><a:kid><a:inner>v</a:inner></a:kid></r>
v
bool(true)
<?xml version="1.0"?>
<r xmlns:a="http://example.com"><a:kid><a:inner>w</a:inner></a:kid></r>
w
<?xml version="1.0"?>
<r><kid><inner>z</inner></kid></r>
z
bool(true)
<?xml version="1.0"?>
<p:r xmlns:p="http://example.com/p"><p:kid id="7"/></p:r>
7
<?xml version="1.0"?>
<r xmlns:a="http://example.com"><a:kid id="8"/></r>
bool(false)
