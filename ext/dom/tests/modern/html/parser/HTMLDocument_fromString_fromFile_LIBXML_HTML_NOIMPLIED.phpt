--TEST--
DOM\HTMLDocument::createFromString()/createFromFile() with LIBXML_HTML_NOIMPLIED
--EXTENSIONS--
dom
--FILE--
<?php
function test(string $html) {
    echo "Testing: $html\n";
    $dom = DOM\HTMLDocument::createFromString($html, LIBXML_HTML_NOIMPLIED | LIBXML_NOERROR);
    $output = $dom->saveHTML();
    echo $output, "\n";

    // Also test the loadHTMLFile variation. We won't print out the result, just checking the result is the same.
    $temp = fopen(__DIR__."/DOM_HTMLDocument_loadHTML_LIBXML_HTML_NOIMPLIED_input.tmp", "w");
    fwrite($temp, $html);
    fclose($temp);
    $dom = DOM\HTMLDocument::createFromFile(__DIR__."/DOM_HTMLDocument_loadHTML_LIBXML_HTML_NOIMPLIED_input.tmp", LIBXML_HTML_NOIMPLIED | LIBXML_NOERROR);
    var_dump($output === $dom->saveHTML());
}

echo "--- Missing html, head, body ---\n";
test("");
test("foobarbaz");
test("<p>foo</p>");
echo "--- Missing html, head ---\n";
test("<body><p>foo</p></body>");
test("<title>x</title><p>foo</p>");
echo "--- Missing html, body ---\n";
test("<!-- bar --><head><title>x</title><!-- foo --></head><p>foo</p><!-- bar -->");
echo "--- Missing html ---\n";
test("<head><title>x</title></head><body><p>foo</p></body>");
echo "--- Missing head, body ---\n";
test("<html>foobar</html>");
test("<html><!-- foo --><title>a</title><p>foo</p></html><!-- bar -->");
echo "--- Missing head ---\n";
test("<html><!-- foo --><body>hi</body></html>");
echo "--- Missing nothing ---\n";
test("<html><head><title>x</title></head><!-- foo --><body><p>foo</p></body></html>");
echo "--- Malformed document ---\n";
test("<!-- start --><body><head><html>foo</html></head></body><!-- end -->");
?>
--CLEAN--
<?php
@unlink(__DIR__."/DOM_HTMLDocument_loadHTML_LIBXML_HTML_NOIMPLIED_input.tmp");
?>
--EXPECT--
--- Missing html, head, body ---
Testing: 

bool(true)
Testing: foobarbaz
foobarbaz
bool(true)
Testing: <p>foo</p>
<p>foo</p>
bool(true)
--- Missing html, head ---
Testing: <body><p>foo</p></body>
<body><p>foo</p></body>
bool(true)
Testing: <title>x</title><p>foo</p>
<title>x</title><p>foo</p>
bool(true)
--- Missing html, body ---
Testing: <!-- bar --><head><title>x</title><!-- foo --></head><p>foo</p><!-- bar -->
<!-- bar --><head><title>x</title><!-- foo --></head><p>foo</p><!-- bar -->
bool(true)
--- Missing html ---
Testing: <head><title>x</title></head><body><p>foo</p></body>
<head><title>x</title></head><body><p>foo</p></body>
bool(true)
--- Missing head, body ---
Testing: <html>foobar</html>
<html>foobar</html>
bool(true)
Testing: <html><!-- foo --><title>a</title><p>foo</p></html><!-- bar -->
<html><!-- foo --><title>a</title><p>foo</p></html><!-- bar -->
bool(true)
--- Missing head ---
Testing: <html><!-- foo --><body>hi</body></html>
<html><!-- foo --><body>hi</body></html>
bool(true)
--- Missing nothing ---
Testing: <html><head><title>x</title></head><!-- foo --><body><p>foo</p></body></html>
<html><head><title>x</title></head><!-- foo --><body><p>foo</p></body></html>
bool(true)
--- Malformed document ---
Testing: <!-- start --><body><head><html>foo</html></head></body><!-- end -->
<!-- start --><body>foo</body><!-- end -->
bool(true)
