<?php

// Common code for tests which focus on conversion and verification of text
// in some specific encoding

// Test failed limit. If you want to execute all tests, set to -1.
$testFailedLimit = 1000;
// Test failed counter
$testFailedCounter = 0;

// Count tests failed. If tests failed are more than $testFailedLimit, stop testing.
function testFailedIncrement() {
    global $testFailedCounter, $testFailedLimit;

    // $testFailedLimit is -1, no limit.
    if ($testFailedLimit === -1)
        return;

    $testFailedCounter++;
    if ($testFailedCounter < $testFailedLimit)
        return;

    die("=== Failed test " . $testFailedLimit . " times exceeded, stop testing ===");
}

// Read a file with one character and its equivalent Unicode codepoint on each
// line, delimited by tabs
function readConversionTable($path, &$from, &$to, $utf32 = false) {
    $from = array();
    $to   = array();

    $fp = fopen($path, 'r+');
    while ($line = fgets($fp, 256)) {
        if ($line[0] == '#')
            continue;
        if (sscanf($line, "0x%x\t0x%x", $char, $codepoint) == 2) {
            // Skip codepoints that do not have a mapping (e.g. in BIG5.txt)
            if ($codepoint === 0xFFFD) {
                continue;
            }
            $codepoint = $utf32 ? pack('N', $codepoint) : pack('n', $codepoint);
            if ($char == PHP_INT_MAX) {
                // We may be on a 32-bit machine and testing a text encoding with 4-byte codes
                // (which can't be represented in a PHP integer)
                $char = "";
                for ($i = 2; $i < strlen($line); $i += 2) {
                    $substr = substr($line, $i, 2);
                    if (ctype_xdigit($substr))
                        $char .= chr(hexdec($substr));
                    else
                        break;
                }
            } else {
                if ($char <= 0xFF)
                    $char = chr($char); // hex codes must not have leading zero bytes
                else if ($char <= 0xFFFF)
                    $char = pack('n', $char);
                else if ($char <= 0xFFFFFF)
                    $char = chr($char >> 16) . pack('n', $char & 0xFFFF);
                else
                    $char = pack('N', $char);
            }
            $from[$char] = $codepoint;
            $to[$codepoint] = $char;
        }
    }
}

function dbgPrint($str) {
    $result = '';
    if (mb_check_encoding($str, 'ASCII'))
        $result .= '"' . $str . '" ';
    return $result . "(" . bin2hex($str) . ")";
}

function identifyValidString($goodString, $encoding) {
    $result = mb_check_encoding($goodString, $encoding);
    if (!$result) {
        echo "mb_check_encoding failed on good $encoding string: " . dbgPrint($goodString) . PHP_EOL;
        testFailedIncrement();
    }
}

function identifyInvalidString($badString, $encoding) {
    $result = mb_check_encoding($badString, $encoding);
    if ($result)
        echo "mb_check_encoding passed on bad $encoding string: " . dbgPrint($badString) . PHP_EOL;
}

function testConversion($fromString, $toString, $fromEncoding, $toEncoding) {
    $result = mb_convert_encoding($fromString, $toEncoding, $fromEncoding);
    if ($result !== $toString) {
        echo "mb_convert_encoding not working on $fromEncoding input: " . dbgPrint($fromString) . "\nExpected $toEncoding: " . dbgPrint($toString) . "\nActually got: " . dbgPrint($result) . PHP_EOL;
        testFailedIncrement();
    }
}

function testValidConversion($fromString, $toString, $fromEncoding, $toEncoding) {
    $illegalChars = mb_get_info('illegal_chars');
    testConversion($fromString, $toString, $fromEncoding, $toEncoding);
    if (mb_get_info('illegal_chars') !== $illegalChars) {
        echo "mb_convert_encoding incremented illegal_chars on valid $fromEncoding string: " . dbgPrint($fromString) . " when converting to $toEncoding" . PHP_EOL;
        testFailedIncrement();
    }
}

function convertValidString($fromString, $toString, $fromEncoding, $toEncoding, $bothWays = true) {
    testValidConversion($fromString, $toString, $fromEncoding, $toEncoding);
    if ($bothWays)
        testValidConversion($toString, $fromString, $toEncoding, $fromEncoding);
}

function convertInvalidString($fromString, $toString, $fromEncoding, $toEncoding) {
    $illegalChars = mb_get_info('illegal_chars');
    testConversion($fromString, $toString, $fromEncoding, $toEncoding);
    if (mb_get_info('illegal_chars') <= $illegalChars) {
        echo "mb_convert_encoding did not increment illegal_chars on invalid $fromEncoding string: " . dbgPrint($fromString) . " when converting to $toEncoding" . PHP_EOL;
        testFailedIncrement();
    }
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
function testAllValidChars($charMap, $fromEncoding, $toEncoding, $bothWays = true) {
    $goodChars = array_keys($charMap);
    shuffle($goodChars);
    // Try a long string
    $fromString = $toString = '';
    foreach ($goodChars as $goodChar) {
        $fromString .= $goodChar;
        $toString .= $charMap[$goodChar];
    }
    testValidString($fromString, $toString, $fromEncoding, $toEncoding, $bothWays);
    // Try various shorter ones
    while (!empty($goodChars)) {
        $length = min(rand(5,10), count($goodChars));
        $fromString = $toString = '';
        while ($length--) {
            $goodChar = array_pop($goodChars);
            $fromString .= $goodChar;
            $toString .= $charMap[$goodChar];
        }

        testValidString($fromString, $toString, $fromEncoding, $toEncoding, $bothWays);

        $strLen = mb_strlen($fromString, $fromEncoding);
        if ($strLen !== mb_strlen($toString, $toEncoding)) {
            echo "Length of $fromEncoding string '" . bin2hex($fromString) . "' was different than expected; mb_strlen returned $strLen" . PHP_EOL;
            testFailedIncrement();
        }
    }
}

function testAllInvalidChars($badChars, $charMap, $fromEncoding, $toEncoding, $replacement) {
    $badChars = array_keys($badChars);
    $goodChars = array();
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

function convertAllInvalidChars($badChars, $charMap, $fromEncoding, $toEncoding, $replacement) {
    $badChars = array_keys($badChars);
    $goodChars = array();
    while (!empty($badChars)) {
        if (empty($goodChars)) {
            $goodChars = array_keys($charMap);
            shuffle($goodChars);
        }
        $goodChar   = array_pop($goodChars);
        $fromString = array_pop($badChars) . $goodChar;
        $toString   = $replacement . $charMap[$goodChar];

        convertInvalidString($fromString, $toString, $fromEncoding, $toEncoding);
    }
}

function testTruncatedChars($truncated, $fromEncoding, $toEncoding, $replacement) {
    $truncatedChars = array_keys($truncated);
    foreach ($truncatedChars as $truncatedChar) {
        testInvalidString($truncatedChar, $replacement, $fromEncoding, $toEncoding);
    }
}

// For variable-width encodings, where we have an exhaustive list of
// all valid characters of any width
//
// `$startBytes` maps from first-byte values to the corresponding character length
// (For encodings where the first byte can tell you the length of a multi-byte
// character)
// Note that `$startBytes` can be partial!
function findInvalidChars($valid, &$invalid, &$truncated, $startBytes = array()) {
    $invalid    = array();
    $truncated  = array();
    $prefixes   = array(); /* All sequences which are not (but can start) a valid character */

    foreach ($valid as $char => $unicode) {
        for ($len = 1; $len < strlen($char); $len++)
            $prefixes[substr($char, 0, $len)] = true;
    }

    $varLength = function($prefix) use($valid, $prefixes, &$invalid, &$truncated, &$varLength) {
        for ($byte = 0; $byte < 256; $byte++) {
            $str = $prefix . chr($byte);
            if (!isset($valid[$str])) {
                if (isset($prefixes[$str])) {
                    $truncated[$str] = true;
                    $varLength($str);
                } else {
                    $invalid[$str] = true;
                }
            }
        }
    };

    $fixedLength = function($prefix, $remaining) use($valid, $prefixes, &$invalid, &$truncated, &$fixedLength) {
        if ($remaining == 0) {
            if (!isset($valid[$prefix]))
                $invalid[$prefix] = true;
        } else if ($remaining == 1) {
            $truncated[$prefix] = true;
            for ($i = 0; $i < 256; $i++) {
                $str = $prefix . chr($i);
                if (!isset($valid[$str]))
                    $invalid[$str] = true;
            }
        } else {
            $truncated[$prefix] = true;
            for ($i = 0; $i < 256; $i++)
                $fixedLength($prefix . chr($i), $remaining - 1);
        }
    };

    for ($byte = 0; $byte < 256; $byte++) {
        if (isset($startBytes[$byte])) {
            $fixedLength(chr($byte), $startBytes[$byte] - 1);
        } else {
            $str = chr($byte);
            if (!isset($valid[$str])) {
                if (isset($prefixes[$str])) {
                    $truncated[$str] = true;
                    $varLength($str);
                } else {
                    $invalid[$str] = true;
                }
            }
        }
    }
}

function testEncodingFromUTF16ConversionTable($path, $encoding, $replacement = '%', $startBytes = array()) {
    srand(1000); // Make results consistent
    mb_substitute_character(0x25); // '%'
    readConversionTable($path, $toUnicode, $fromUnicode);

    findInvalidChars($toUnicode, $invalid, $truncated, $startBytes);
    testAllValidChars($toUnicode, $encoding, 'UTF-16BE');
    testAllInvalidChars($invalid, $toUnicode, $encoding, 'UTF-16BE', "\x00%");
    testTruncatedChars($truncated, $encoding, 'UTF-16BE', "\x00%");
    echo "Tested $encoding -> UTF-16BE\n";

    findInvalidChars($fromUnicode, $invalid, $unused, array_fill_keys(range(0,0xFF), 2));
    convertAllInvalidChars($invalid, $fromUnicode, 'UTF-16BE', $encoding, $replacement);
    echo "Tested UTF-16BE -> $encoding\n";
}
?>
