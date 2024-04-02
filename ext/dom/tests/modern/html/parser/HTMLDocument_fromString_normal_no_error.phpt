--TEST--
DOM\HTMLDocument::createFromString() - normal document, no error
--EXTENSIONS--
dom
--FILE--
<?php

// The closing p tag breaks libxml2's HTML parser, but doesn't break the HTML5 parser due to the script context parsing rules.
$html = <<<HTML
<!DOCTYPE HTML>
<html>
    <head>
        <meta charset="utf-8">
        <title>foo</title>
    </head>
    <body>
        <script>
        var foo = "</p>";
        </script>
        <p test="<script>">bar <!-- hi --></p>
    </body>
</html>
HTML;
$dom = DOM\HTMLDocument::createFromString($html);
echo $dom->saveHTML(), "\n";

?>
--EXPECT--
<!DOCTYPE html><html><head>
        <meta charset="utf-8">
        <title>foo</title>
    </head>
    <body>
        <script>
        var foo = "</p>";
        </script>
        <p test="<script>">bar <!-- hi --></p>
    
</body></html>
