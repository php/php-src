--TEST--
DOMEntity fields
--EXTENSIONS--
dom
--FILE--
<?php
$xmlString = <<<XML
<?xml version="1.0"?>
<!DOCTYPE test [
    <!ENTITY sampleInternalEntity "This is a sample entity value.">
    <!ENTITY sampleExternalSystemWithNotationName SYSTEM "external.stuff" NDATA stuff>
    <!ENTITY sampleExternalSystemWithoutNotationName SYSTEM "external.stuff" NDATA >
    <!ENTITY sampleExternalPublicWithNotationName1 PUBLIC "public id" "external.stuff" NDATA stuff>
    <!ENTITY sampleExternalPublicWithNotationName2 PUBLIC "" "external.stuff" NDATA stuff>
    <!ENTITY sampleExternalPublicWithoutNotationName1 PUBLIC "public id" "external.stuff" NDATA >
    <!ENTITY sampleExternalPublicWithoutNotationName2 PUBLIC "" "external.stuff" NDATA >
]>
<root/>
XML;

$dom = new DOMDocument();
$dom->loadXML($xmlString);

// Sort them, the iteration order isn't defined
$entities = iterator_to_array($dom->doctype->entities);
ksort($entities);

foreach ($entities as $entity) {
    echo "Entity name: {$entity->nodeName}\n";
    echo "publicId: ";
    var_dump($entity->publicId);
    echo "systemId: ";
    var_dump($entity->systemId);
    echo "notationName: ";
    var_dump($entity->notationName);
    echo "actualEncoding: ";
    var_dump($entity->actualEncoding);
    echo "encoding: ";
    var_dump($entity->encoding);
    echo "version: ";
    var_dump($entity->version);
    echo "\n";
}
?>
--EXPECTF--
Entity name: sampleExternalPublicWithNotationName1
publicId: string(9) "public id"
systemId: string(14) "external.stuff"
notationName: string(5) "stuff"
actualEncoding: 
Deprecated: Property DOMEntity::$actualEncoding is deprecated in %s on line %d
NULL
encoding: 
Deprecated: Property DOMEntity::$encoding is deprecated in %s on line %d
NULL
version: 
Deprecated: Property DOMEntity::$version is deprecated in %s on line %d
NULL

Entity name: sampleExternalPublicWithNotationName2
publicId: string(0) ""
systemId: string(14) "external.stuff"
notationName: string(5) "stuff"
actualEncoding: 
Deprecated: Property DOMEntity::$actualEncoding is deprecated in %s on line %d
NULL
encoding: 
Deprecated: Property DOMEntity::$encoding is deprecated in %s on line %d
NULL
version: 
Deprecated: Property DOMEntity::$version is deprecated in %s on line %d
NULL

Entity name: sampleExternalPublicWithoutNotationName1
publicId: string(9) "public id"
systemId: string(14) "external.stuff"
notationName: string(0) ""
actualEncoding: 
Deprecated: Property DOMEntity::$actualEncoding is deprecated in %s on line %d
NULL
encoding: 
Deprecated: Property DOMEntity::$encoding is deprecated in %s on line %d
NULL
version: 
Deprecated: Property DOMEntity::$version is deprecated in %s on line %d
NULL

Entity name: sampleExternalPublicWithoutNotationName2
publicId: string(0) ""
systemId: string(14) "external.stuff"
notationName: string(0) ""
actualEncoding: 
Deprecated: Property DOMEntity::$actualEncoding is deprecated in %s on line %d
NULL
encoding: 
Deprecated: Property DOMEntity::$encoding is deprecated in %s on line %d
NULL
version: 
Deprecated: Property DOMEntity::$version is deprecated in %s on line %d
NULL

Entity name: sampleExternalSystemWithNotationName
publicId: NULL
systemId: string(14) "external.stuff"
notationName: string(5) "stuff"
actualEncoding: 
Deprecated: Property DOMEntity::$actualEncoding is deprecated in %s on line %d
NULL
encoding: 
Deprecated: Property DOMEntity::$encoding is deprecated in %s on line %d
NULL
version: 
Deprecated: Property DOMEntity::$version is deprecated in %s on line %d
NULL

Entity name: sampleExternalSystemWithoutNotationName
publicId: NULL
systemId: string(14) "external.stuff"
notationName: string(0) ""
actualEncoding: 
Deprecated: Property DOMEntity::$actualEncoding is deprecated in %s on line %d
NULL
encoding: 
Deprecated: Property DOMEntity::$encoding is deprecated in %s on line %d
NULL
version: 
Deprecated: Property DOMEntity::$version is deprecated in %s on line %d
NULL

Entity name: sampleInternalEntity
publicId: NULL
systemId: NULL
notationName: NULL
actualEncoding: 
Deprecated: Property DOMEntity::$actualEncoding is deprecated in %s on line %d
NULL
encoding: 
Deprecated: Property DOMEntity::$encoding is deprecated in %s on line %d
NULL
version: 
Deprecated: Property DOMEntity::$version is deprecated in %s on line %d
NULL
