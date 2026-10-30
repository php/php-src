--TEST--
DOMDocument::saveHTMLFile() does not close the encoder twice when opening the output fails
--EXTENSIONS--
dom
--FILE--
<?php
$filename = __DIR__ . '/missing-saveHTMLFile-directory/output.html';
foreach (['UTF-8', 'ISO-8859-1', 'UTF-16'] as $encoding) {
    $doc = new DOMDocument();
    $doc->loadHTML('<html><head><meta http-equiv="Content-Type" content="text/html; charset=UTF-8"></head><body>value</body></html>');
    $doc->getElementsByTagName('meta')->item(0)->setAttribute('content', 'text/html; charset=' . $encoding);
    for ($i = 0; $i < 3; $i++) {
        $result = @$doc->saveHTMLFile($filename);
        var_dump($result === 0 || $result === false);
    }
    var_dump($doc->getElementsByTagName('body')->item(0)->textContent === 'value');
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
