--TEST--
DOM\HTMLDocument::createFromString() - parser warning 03
--EXTENSIONS--
dom
--FILE--
<?php

$html = file_get_contents(__DIR__."/parser_warning_03.html");
$dom = DOM\HTMLDocument::createFromString($html, LIBXML_NOERROR);
echo $dom->saveHTML(), "\n";

?>
--EXPECT--
<!DOCTYPE html><html><head></head><body>
        <p>
    
</p></body></html>
