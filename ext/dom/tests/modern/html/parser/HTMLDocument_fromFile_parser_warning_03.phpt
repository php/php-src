--TEST--
DOM\HTMLDocument::createFromFile() - parser warning 03
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromFile(__DIR__."/parser_warning_03.html", LIBXML_NOERROR);
echo $dom->saveHTML(), "\n";

?>
--EXPECT--
<!DOCTYPE html><html><head></head><body>
        <p>
    
</p></body></html>
