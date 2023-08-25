--TEST--
DOM\HTML5Document::loadHTML() - parser warning 02
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();
$html = file_get_contents(__DIR__."/parser_warning_02.html");
$dom->loadHTML($html);
echo $dom->saveHTML(), "\n";

?>
--EXPECTF--
Warning: DOM\HTML5Document::loadHTML(): tokenizer error unexpected-null-character in Entity, line: 4, column: 11 in %s on line %d

Warning: DOM\HTML5Document::loadHTML(): tokenizer error missing-whitespace-between-attributes in Entity, line: 5, column: 20 in %s on line %d

Warning: DOM\HTML5Document::loadHTML(): tokenizer error incorrectly-opened-comment in Entity, line: 6, column: 11 in %s on line %d

Warning: DOM\HTML5Document::loadHTML(): tokenizer error nested-comment in Entity, line: 7, column: 18 in %s on line %d

Warning: DOM\HTML5Document::loadHTML(): tree error unexpected-closed-token in Entity, line: 4, column: 18 in %s on line %d

Warning: DOM\HTML5Document::loadHTML(): tree error doctype-token-in-body-mode in Entity, line: 8, column: 11-17 in %s on line %d
<!DOCTYPE html><html><head></head><body>
        <p�>foo<p></p>
        <p id="foo" class="bar">
        <!--ELEMENT br EMPTY-->
        <!-- <!-- nested --> --&gt;
        
    
</p></p�></body></html>
