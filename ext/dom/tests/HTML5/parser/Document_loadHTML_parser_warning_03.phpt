--TEST--
DOM\HTML5Document::loadHTML() - parser warning 03
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();
$html = file_get_contents(__DIR__."/parser_warning_03.html");
$dom->loadHTML($html, LIBXML_NOERROR);
echo $dom->saveHTML(), "\n";

?>
--EXPECT--
<!DOCTYPE html><html><head></head><body>
        <p>
    
</p></body></html>
