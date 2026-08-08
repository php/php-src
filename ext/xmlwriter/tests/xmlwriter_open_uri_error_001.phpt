--TEST--
xmlwriter_open_uri with empty string as parameter
--EXTENSIONS--
xmlwriter
--FILE--
<?php
try {
    xmlwriter_open_uri('');
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}
?>
--CREDITS--
Koen Kuipers koenk82@gmail.com
Theo van der Zee
#Test Fest Utrecht 09-05-2009
--EXPECT--
ValueError: xmlwriter_open_uri(): Argument #1 ($uri) must not be empty
