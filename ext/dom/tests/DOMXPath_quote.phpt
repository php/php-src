--TEST--
Test DOMXPath::quote with various inputs
--EXTENSIONS--
dom
--SKIPIF--
<?php if (!class_exists('DOMXPath')) die('skip DOMXPath not available.'); ?>
--FILE--
<?php
$dom = new DOMDocument();
$xpath = new DOMXPath($dom);


/**
 * Quote a string for use in an XPath expression.
 * 
 * Example: $xp->query("//span[contains(text()," . $xp->quote($string) . ")]")
 * 
 * @param string $string string to quote.
 * @return string quoted string.
 */
function UserlandDOMXPathQuote(string $string): string
{
    if (false === \strpos($string, '\'')) {
        return '\'' . $string . '\'';
    }
    if (false === \strpos($string, '"')) {
        return '"' . $string . '"';
    }
    // if the string contains both single and double quotes, construct an
    // expression that concatenates all non-double-quote substrings with
    // the quotes, e.g.:
    //  'foo'"bar => concat("'foo'", '"bar")
    $sb = [];
    while ($string !== '') {
        $bytesUntilSingleQuote = \strcspn($string, '\'');
        $bytesUntilDoubleQuote = \strcspn($string, '"');
        $quoteMethod = ($bytesUntilSingleQuote > $bytesUntilDoubleQuote) ? "'" : '"';
        $bytesUntilQuote = \max($bytesUntilSingleQuote, $bytesUntilDoubleQuote);
        $sb[] = $quoteMethod . \substr($string, 0, $bytesUntilQuote) . $quoteMethod;
        $string = \substr($string, $bytesUntilQuote);
    }
    $sb = \implode(',', $sb);
    return 'concat(' . $sb . ')';
}



$tests = [
    'foo' => "'foo'", // no quotes
    '"foo' => '\'"foo\'', // double quotes only
    '\'foo' => '"\'foo"', // single quotes only
    '\'foo"bar' => 'concat("\'foo",\'"bar\')', // both; double quotes in mid-string
    '\'foo"bar"baz' => 'concat("\'foo",\'"bar"baz\')', // multiple double quotes in mid-string
    '\'foo"' => 'concat("\'foo",\'"\')', // string ends with double quotes
    '\'foo""' => 'concat("\'foo",\'""\')', // string ends with run of double quotes
    '"\'foo' => 'concat(\'"\',"\'foo")', // string begins with double quotes
    '""\'foo' => 'concat(\'""\',"\'foo")', // string begins with run of double quotes
    '\'foo""bar' => 'concat("\'foo",\'""bar\')', // run of double quotes in mid-string
];

foreach ($tests as $input => $expected) {
    $result = $xpath->quote($input);
    if ($result === $expected) {
        echo "Pass: {$input} => {$result}\n";
    } else {
        echo 'Fail: ';
        var_dump([
            'input' => $input,
            'expected' => $expected,
            'result' => $result,
            'userland_implementation_result' => UserlandDOMXPathQuote($input),
        ]);
    }
}
?>
--EXPECT--
Pass: foo => 'foo'
Pass: "foo => '"foo'
Pass: 'foo => "'foo"
Pass: 'foo"bar => concat("'foo",'"bar')
Pass: 'foo"bar"baz => concat("'foo",'"bar"baz')
Pass: 'foo" => concat("'foo",'"')
Pass: 'foo"" => concat("'foo",'""')
Pass: "'foo => concat('"',"'foo")
Pass: ""'foo => concat('""',"'foo")
Pass: 'foo""bar => concat("'foo",'""bar')
