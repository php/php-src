--TEST--
xmlwriter_open_uri with empty string as parameter
--SKIPIF--
<?php if (!extension_loaded("xmlwriter")) print "skip"; ?>
--FILE--
<?php
try {
    xmlwriter_open_uri('');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--CREDITS--
Koen Kuipers koenk82@gmail.com
Theo van der Zee
#Test Fest Utrecht 09-05-2009
--EXPECT--
xmlwriter_open_uri(): Argument #1 ($uri) cannot be empty
