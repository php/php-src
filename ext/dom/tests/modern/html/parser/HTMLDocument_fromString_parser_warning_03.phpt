--TEST--
Dom\HTMLDocument::createFromString() - parser warning 03
--EXTENSIONS--
dom
--FILE--
<?php

$html = file_get_contents(__DIR__."/parser_warning_03.html");
$dom = Dom\HTMLDocument::createFromString($html, LIBXML_NOERROR);
echo $dom->saveHtml(), "\n";

?>
--EXPECT--
<!DOCTYPE html><html><head></head><body>
        <p>
    
</p></body></html>
