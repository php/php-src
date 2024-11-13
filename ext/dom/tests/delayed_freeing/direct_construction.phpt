--TEST--
Tests with direction construction
--EXTENSIONS--
dom
--FILE--
<?php
function node_alike_test($test) {
    try {
        var_dump($test->parentNode);
        var_dump($test->nodeValue);
    } catch (Throwable $e) {
        echo $e->getMessage(), "\n";
    }
    try {
        var_dump($test->appendChild($test));
    } catch (Throwable $e) {
        echo $e->getMessage(), "\n";
    }
}

echo "-- Test DOMCharacterData --\n";
$test = new DOMCharacterData("test");
try {
    var_dump($test->textContent);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($test->appendData('test'));
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

echo "-- Test DOMCdataSection --\n";
$test = new DOMCdataSection("test");
var_dump($test->textContent);
var_dump($test->appendData('test'));

echo "-- Test DOMText --\n";
$test = new DOMText("test");
try {
    var_dump($test->wholeText);
    var_dump($test->parentNode);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($test->isWhitespaceInElementContent());
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

echo "-- Test DOMComment --\n";
$test = new DOMComment("my comment");
var_dump($test->textContent);
var_dump($test->parentNode);
var_dump($test->getLineNo());

echo "-- Test DOMElement --\n";
$test = new DOMElement("qualifiedName", "test");
var_dump($test->textContent);
var_dump($test->parentNode);
try {
    var_dump($test->appendChild($test));
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

echo "-- Test DOMNode --\n";
node_alike_test(new DOMNode());

echo "-- Test DOMNotation --\n";
node_alike_test(new DOMNotation());

echo "-- Test DOMProcessingInstruction --\n";
node_alike_test(new DOMProcessingInstruction("name", "value"));

echo "-- Test DOMEntity --\n";
$test = new DOMEntity();
try {
    var_dump($test->nodeValue);
    var_dump($test->parentNode);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($test->appendChild($test));
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

echo "-- Test DOMAttr --\n";
$test = new DOMAttr("attr", "value");
var_dump($test->nodeValue);
var_dump($test->parentNode);
try {
    var_dump($test->appendChild($test));
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
-- Test DOMCharacterData --
Invalid State Error
Couldn't fetch DOMCharacterData
-- Test DOMCdataSection --
string(4) "test"
bool(true)
-- Test DOMText --
string(4) "test"
NULL
bool(false)
-- Test DOMComment --
string(10) "my comment"
NULL
int(0)
-- Test DOMElement --
string(4) "test"
NULL
No Modification Allowed Error
-- Test DOMNode --
Invalid State Error
Couldn't fetch DOMNode
-- Test DOMNotation --
Invalid State Error
Couldn't fetch DOMNotation
-- Test DOMProcessingInstruction --
NULL
string(5) "value"
bool(false)
-- Test DOMEntity --
Invalid State Error
Couldn't fetch DOMEntity
-- Test DOMAttr --
string(5) "value"
NULL
No Modification Allowed Error
