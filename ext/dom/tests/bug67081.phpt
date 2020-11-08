--TEST--
Bug #67081 DOMDocumentType->internalSubset returns entire DOCTYPE tag, not only the subset
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php
    $domDocument = new DOMDocument();
    $domDocument->substituteEntities = true;
    $domDocument->load(__DIR__ . DIRECTORY_SEPARATOR . "bug67081_0.xml");
    var_dump($domDocument->doctype->internalSubset);

    $domDocument = new DOMDocument();
    $domDocument->substituteEntities = true;
    $domDocument->load(__DIR__ . DIRECTORY_SEPARATOR . "bug67081_1.xml");
    var_dump($domDocument->doctype->internalSubset);

    $domDocument = new DOMDocument();
    $domDocument->substituteEntities = true;
    $domDocument->load(__DIR__ . DIRECTORY_SEPARATOR . "bug67081_2.xml");
    var_dump($domDocument->doctype->internalSubset);

    $domDocument = new DOMDocument();
    $domDocument->substituteEntities = true;
    $domDocument->load(__DIR__ . DIRECTORY_SEPARATOR . "dom.xml");
    var_dump($domDocument->doctype->internalSubset);
?>
--EXPECT--
string(19) "<!ELEMENT a EMPTY>
"
string(38) "<!ELEMENT a EMPTY>
<!ELEMENT b EMPTY>
"
NULL
string(277) "<!ENTITY % incent SYSTEM "dom.ent">
<!ENTITY amp "&#38;#38;">
<!ENTITY gt "&#62;">
<!ENTITY % coreattrs "title CDATA #IMPLIED">
<!ENTITY % attrs "%coreattrs;">
<!ATTLIST foo bar CDATA #IMPLIED>
<!ELEMENT foo (#PCDATA)>
<!ELEMENT root (foo)+>
<!ATTLIST th title CDATA #IMPLIED>
"
