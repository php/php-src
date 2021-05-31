--TEST--
DOMDocumentType::publicId with invalid state.
--CREDITS--
Eric Lee Stewart <ericleestewart@gmail.com>
# TestFest Atlanta 2009-05-25
--EXTENSIONS--
dom
--FILE--
<?php
$doctype = new DOMDocumentType();
try {
    $publicId = $doctype->publicId;
} catch (DOMException $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
Invalid State Error
