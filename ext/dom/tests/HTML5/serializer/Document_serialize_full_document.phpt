--TEST--
DOM\HTML5Document serialization of full document
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();
$dom->loadHTML(<<<HTML
<!DOCTYPE HTML>
<html lang="en">
    <head>
        <title>This is my épic title!</title>
        <meta charset="utf-8">
        <script>
        var foo="</p>";
        </script>
        <style>
        foo > bar {}
        </style>
    </head>
    <body>
        <main xmlns="some:ns" foo:bar="x" normal="x">
            <img alt="foo" src="/">
            <br>
            <test:test></test:test>
        </main>
    </body>
</html>
HTML);
echo $dom->saveHTML();

?>
--EXPECT--
<!DOCTYPE html><html lang="en"><head>
        <title>This is my épic title!</title>
        <meta charset="utf-8">
        <script>
        var foo="</p>";
        </script>
        <style>
        foo > bar {}
        </style>
    </head>
    <body>
        <main xmlns="some:ns" foo:bar="x" normal="x">
            <img alt="foo" src="/">
            <br>
            <test:test></test:test>
        </main>
    
</body></html>
