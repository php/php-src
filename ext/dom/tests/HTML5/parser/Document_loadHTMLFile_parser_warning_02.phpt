--TEST--
DOM\HTML5Document::loadHTMLFile() - parser warning 02
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();
$dom->loadHTMLFile(__DIR__."/parser_warning_02.html", LIBXML_NOERROR);
echo $dom->saveHTML(), "\n";

?>
--EXPECT--
<!DOCTYPE html><html><head></head><body>
        <p�>foo<p></p>
        <p id="foo" class="bar">
        <!--ELEMENT br EMPTY-->
        <!-- <!-- nested --> --&gt;
        
    
</p></p�></body></html>
