--TEST--
DomDocument::$strictErrorChecking - ensure turning off actually works
--CREDITS--
Vincent Tsao <notes4vincent@gmail.com>
(and Dan Convissor)
# TestFest 2009 NYPHP
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

echo "Load document\n";
$doc = new DOMDocument;
$doc->load(__DIR__."/book.xml");

echo "See if strictErrorChecking is on\n";
var_dump($doc->strictErrorChecking);

echo "Should throw DOMException when strictErrorChecking is on\n";
try {
    $attr = $doc->createAttribute(0);
} catch (DOMException $e) {
    echo "GOOD. DOMException thrown\n";
    echo $e->getMessage() ."\n";
} catch (Exception $e) {
    echo "OOPS. Other exception thrown\n";
}


echo "Turn strictErrorChecking off\n";
$doc->strictErrorChecking = false;

echo "See if strictErrorChecking is off\n";
var_dump($doc->strictErrorChecking);

echo "Should raise PHP error because strictErrorChecking is off\n";
try {
    $attr = $doc->createAttribute(0);
} catch (DOMException $e) {
    echo "OOPS. DOMException thrown\n";
    echo $e->getMessage() ."\n";
} catch (Exception $e) {
    echo "OOPS. Other exception thrown\n";
}

?>
--EXPECTF--
Load document
See if strictErrorChecking is on
bool(true)
Should throw DOMException when strictErrorChecking is on
GOOD. DOMException thrown
Invalid Character Error
Turn strictErrorChecking off
See if strictErrorChecking is off
bool(false)
Should raise PHP error because strictErrorChecking is off

Warning: DOMDocument::createAttribute(): Invalid Character Error in %sDOMDocument_strictErrorChecking_variation.php on line %d
