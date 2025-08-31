--TEST--
DOMDocumentType: basic access to all properties.
--CREDITS--
Eric Lee Stewart <ericleestewart@gmail.com>
# TestFest Atlanta 2009-05-25
--EXTENSIONS--
dom
--FILE--
<?php
// Access publicId, systemId, name, internalSubset all with values.
$xml  = '<?xml version="1.0" encoding="UTF-8" ?>';
$xml .= '<!DOCTYPE chapter PUBLIC "-//OASIS//DTD DocBook XML//EN" "docbookx.dtd">';
$xml .= '<chapter>1</chapter>';
$doc = new DOMDocument();
$doc->loadXML($xml);
$doctype = $doc->doctype;
print "publicId: ".$doctype->publicId."\n";
print "systemId: ".$doctype->systemId."\n";
print "name: ".$doctype->name."\n";
print "internalSubset: ".$doctype->internalSubset."\n";


// Access entities and notations with values.
$xml  = '<?xml version="1.0" encoding="UTF-8" ?>';
$xml .= '<!DOCTYPE img [';
$xml .= '  <!ELEMENT img EMPTY>';
$xml .= '  <!ATTLIST img src ENTITY #REQUIRED>';
$xml .= '  <!ENTITY logo SYSTEM "http://www.xmlwriter.net/logo.gif" NDATA gif>';
$xml .= '  <!NOTATION gif PUBLIC "gif viewer">';
$xml .= ']>';
$xml .= '<img src="logo"/>';
$doc = new DOMDocument();
$doc->loadXML($xml);
$doctype = $doc->doctype;
$entities = $doctype->entities;
$entity = $entities->item(0);
print 'entity: '.$entity->nodeName."\n";
$notations = $doctype->notations;
$notation = $notations->item(0);
print 'notation: '.$notation->nodeName."\n";
?>
--EXPECT--
publicId: -//OASIS//DTD DocBook XML//EN
systemId: docbookx.dtd
name: chapter
internalSubset: 
entity: logo
notation: gif
