<?php

// Common code for tests which focus on conversion and verification of text
// in some specific encoding

function dbgPrint($str) {
    $result = '';
    if (mb_check_encoding($str, 'ASCII'))
        $result .= '"' . $str . '" ';
    return $result . "(" . bin2hex($str) . ")";
}

function identifyValidString($goodString, $encoding) {
    $result = mb_check_encoding($goodString, $encoding);
    if (!$result)
        die("mb_check_encoding failed on good $encoding string: " . dbgPrint($goodString));
}

function identifyInvalidString($badString, $encoding) {
    $result = mb_check_encoding($badString, $encoding);
    if ($result)
        die("mb_check_encoding passed on bad $encoding string: " . dbgPrint($badString));
}

function testConversion($fromString, $toString, $fromEncoding, $toEncoding) {
    $result = mb_convert_encoding($fromString, $toEncoding, $fromEncoding);
    if ($result !== $toString)
        die("mb_convert_encoding not working on $fromEncoding input: " . dbgPrint($fromString) . "\nExpected $toEncoding: " . dbgPrint($toString) . "\nActually got: " . dbgPrint($result));
}

function testValidConversion($fromString, $toString, $fromEncoding, $toEncoding) {
    $illegalChars = mb_get_info('illegal_chars');
    testConversion($fromString, $toString, $fromEncoding, $toEncoding);
    if (mb_get_info('illegal_chars') !== $illegalChars)
        die("mb_convert_encoding incremented illegal_chars on valid $fromEncoding string: " . dbgPrint($fromString) . " when converting to $toEncoding");
}

function convertValidString($fromString, $toString, $fromEncoding, $toEncoding, $bothWays = true) {
    testValidConversion($fromString, $toString, $fromEncoding, $toEncoding);
    if ($bothWays)
        testValidConversion($toString, $fromString, $toEncoding, $fromEncoding);
}

function convertInvalidString($fromString, $toString, $fromEncoding, $toEncoding) {
    $illegalChars = mb_get_info('illegal_chars');
    testConversion($fromString, $toString, $fromEncoding, $toEncoding);
    if (mb_get_info('illegal_chars') <= $illegalChars)
        die("mb_convert_encoding did not increment illegal_chars on invalid $fromEncoding string: " . dbgPrint($fromString) . " when converting to $toEncoding");
}

function testValidString($fromString, $toString, $fromEncoding, $toEncoding, $bothWays = true) {
    identifyValidString($fromString, $fromEncoding);
    convertValidString($fromString, $toString, $fromEncoding, $toEncoding, $bothWays);
}

function testInvalidString($fromString, $toString, $fromEncoding, $toEncoding) {
    identifyInvalidString($fromString, $fromEncoding);
    convertInvalidString($fromString, $toString, $fromEncoding, $toEncoding);
}

// Only for encodings where valid characters can be concatenated together in any
// way, without any escape sequences
// Also only for encodings which can be converted losslessly to and from $toEncoding
function testAllValidChars($charMap, $fromEncoding, $toEncoding) {
    $goodChars = array_keys($charMap);
    shuffle($goodChars);
    while (!empty($goodChars)) {
        $length = min(rand(5,10), count($goodChars));
        $fromString = $toString = '';
        while ($length--) {
            $goodChar = array_pop($goodChars);
            $fromString .= $goodChar;
            $toString .= $charMap[$goodChar];
        }

        testValidString($fromString, $toString, $fromEncoding, $toEncoding);
    }
}

function testAllInvalidChars($badChars, $charMap, $fromEncoding, $toEncoding, $replacement) {
    $badChars = array_keys($badChars);
    shuffle($badChars);
    $goodChars = array_keys($charMap);
    shuffle($goodChars);
    while (!empty($badChars)) {
        if (empty($goodChars)) {
            $goodChars = array_keys($charMap);
            shuffle($goodChars);
        }
        $goodChar   = array_pop($goodChars);
        $fromString = array_pop($badChars) . $goodChar;
        $toString   = $replacement . $charMap[$goodChar];

        testInvalidString($fromString, $toString, $fromEncoding, $toEncoding);
    }
}

?>
