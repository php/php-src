#!/usr/bin/env php
<?php

/**
 * This script checks the constants defined in the curl PHP extension, against those documented on the cURL website:
 * https://curl.se/libcurl/c/symbols-in-versions.html
 *
 * See the discussion at: https://github.com/php/php-src/pull/2961
 */

const CURL_DOC_FILE = 'https://curl.se/libcurl/c/symbols-in-versions.html';

const SOURCE_FILE = __DIR__ . '/curl_arginfo.h';

const MIN_SUPPORTED_CURL_VERSION = '7.29.0';

const IGNORED_CURL_CONSTANTS = [
    'CURLOPT_PROGRESSDATA',
    'CURLOPT_XFERINFODATA',
];

const IGNORED_PHP_CONSTANTS = [
    'CURLOPT_BINARYTRANSFER',
    'CURLOPT_RETURNTRANSFER',
    'CURLOPT_SAFE_UPLOAD',
];

const CONSTANTS_REGEX_PATTERN = '~^CURL(?:OPT|_VERSION|_HTTP)_[A-Z0-9_]+$~';

/**
 * A simple helper to create ASCII tables.
 * It assumes that the same number of columns is always given to add().
 */
class AsciiTable
{
    /**
     * @var array
     */
    private $values = [];

    /**
     * @var array
     */
    private $length = [];

    /**
     * @var int
     */
    private $padding = 4;

    /**
     * @param string[] $values
     *
     * @return void
     */
    public function add(string ...$values) : void
    {
        $this->values[] = $values;

        foreach ($values as $key => $value) {
            $length = strlen($value);

            if (isset($this->length[$key])) {
                $this->length[$key] = max($this->length[$key], $length);
            } else {
                $this->length[$key] = $length;
            }
        }
    }

    /**
     * @return string
     */
    public function __toString() : string
    {
        $result = '';

        foreach ($this->values as $values) {
            foreach ($values as $key => $value) {
                if ($key !== 0) {
                    $result .= str_repeat(' ', $this->padding);
                }

                $result .= str_pad($value, $this->length[$key]);
            }

            $result .= "\n";
        }

        return $result;
    }
}

$curlConstants   = getCurlConstants();
$sourceConstants = getSourceConstants();

$notInPHP  = []; // In cURL doc, but missing from PHP
$notInCurl = []; // In the PHP source, but not in the cURL doc
$outdated  = []; // In the PHP source, but removed before the minimum supported cURL version

foreach ($curlConstants as $name => [$introduced, $deprecated, $removed]) {
    $inPHP = in_array($name, $sourceConstants);

    if ($removed !== null) {
        if (version_compare($removed, MIN_SUPPORTED_CURL_VERSION) <= 0) {
            // constant removed before the minimum supported version
            continue;
        }
    }

    if (! $inPHP) {
        $notInPHP[$name] = [$introduced, $removed];
    }
}

foreach ($sourceConstants as $name) {
    if (! isset($curlConstants[$name])) {
        $notInCurl[] = $name;
        continue;
    }

    $removed = $curlConstants[$name][2];

    if ($removed === null) {
        continue;
    }

    if (version_compare($removed, MIN_SUPPORTED_CURL_VERSION) <= 0) {
        // constant removed before the minimum supported version
        $outdated[$name] = $removed;
    }
}

$allGood = true;

if ($notInPHP) {
    uasort($notInPHP, function($a, $b) {
        return version_compare($a[0], $b[0]);
    });

    $table = new AsciiTable();
    $table->add('Constant', 'Introduced', '', 'Removed', '');

    foreach ($notInPHP as $name => [$introduced, $removed]) {
        if ($removed === null) {
            $removed = '';
            $removedHex = '';
        } else {
            $removedHex = getHexVersion($removed);
        }

        $table->add($name, $introduced, getHexVersion($introduced), $removed, $removedHex);
    }

    echo "Constants missing from the PHP source:\n\n";
    echo $table, "\n";

    $allGood = false;
}

if ($notInCurl) {
    $table = new AsciiTable();

    foreach ($notInCurl as $name) {
        $table->add($name);
    }

    echo "Constants defined in the PHP source, but absent from the cURL documentation:\n\n";
    echo $table, "\n";

    $allGood = false;
}

if ($outdated) {
    uasort($outdated, function($a, $b) {
        return version_compare($a, $b);
    });

    $table = new AsciiTable();
    $table->add('Constant', 'Removed');

    foreach ($outdated as $name => $version) {
        $table->add($name, $version);
    }

    echo "Constants defined in the PHP source, but removed before the minimum supported cURL version:\n\n";
    echo $table, "\n";

    $allGood = false;
}

if ($allGood) {
    echo "All good! Source code and cURL documentation are in sync.\n";
}

/**
 * Loads and parses the cURL constants from the online documentation.
 *
 * The result is an associative array where the key is the constant name, and the value is a numeric array with:
 * - the introduced version
 * - the deprecated version (nullable)
 * - the removed version (nullable)
 *
 * @return array
 */
function getCurlConstants() : array
{
    $html = file_get_contents(CURL_DOC_FILE);

    // Extract the constant list from the HTML file (located in the only <pre> tag in the page)
    preg_match('~<table>(.*?)</table>~s', $html, $matches);
    $constantList = $matches[1];

    /**
     * Parse the cURL constant lines. Possible formats:
     *
     * Name                            Introduced    Deprecated  Removed
     * CURLOPT_CRLFILE                 7.19.0
     * CURLOPT_DNS_USE_GLOBAL_CACHE    7.9.3         7.11.1
     * CURLOPT_FTPASCII                7.1           7.11.1      7.15.5
     * CURLOPT_HTTPREQUEST             7.1           -           7.15.5
     */
    $regexp = '@<tr><td>(?:<a href=".*?">)?(?<const>[A-Za-z0-9_]+)(?:</a>)?</td><td>(?:<a href=".*?">)?(?<added>[\d\.]+)(?:</a>)?</td><td>(?:<a href=".*?">)?(?<deprecated>[\d\.]+)?(?:</a>)?</td><td>(<a href=".*?">)?(?<removed>[\d\.]+)?(</a>)?</td></tr>@m';
    preg_match_all($regexp, $constantList, $matches, PREG_SET_ORDER);

    $constants = [];

    foreach ($matches as $match) {
        $name       = $match['const'];
        $introduced = $match['added'];
        $deprecated = $match['deprecated'] ?? null;
        $removed    = $match['removed'] ?? null;

        if (in_array($name, IGNORED_CURL_CONSTANTS, true) || !preg_match(CONSTANTS_REGEX_PATTERN, $name)) {
            // not a wanted constant
            continue;
        }

        if ($deprecated === '-') { // deprecated version can be a hyphen
            $deprecated = null;
        }

        $constants[$name] = [$introduced, $deprecated, $removed];
    }

    return $constants;
}

/**
 * Parses the defined cURL constants from the PHP extension source code.
 *
 * The result is a numeric array whose values are the constant names.
 *
 * @return array
 */
function getSourceConstants() : array
{
    $source = file_get_contents(SOURCE_FILE);

    preg_match_all('/REGISTER_LONG_CONSTANT\(\"\w+\", (\w+), .+\)/', $source, $matches);

    $constants = [];

    foreach ($matches[1] as $name) {
        if ($name === '__c') { // macro
            continue;
        }

        if (in_array($name, IGNORED_PHP_CONSTANTS, true) || !preg_match(CONSTANTS_REGEX_PATTERN, $name)) {
            // not a wanted constant
            continue;
        }

        $constants[] = $name;
    }

    return $constants;
}

/**
 * Converts a version number to its hex representation as used in the extension source code.
 *
 * Example: 7.25.1 => 0x071901
 *
 * @param string $version
 *
 * @return string
 *
 * @throws \RuntimeException
 */
function getHexVersion(string $version) : string
{
    $parts = explode('.', $version);

    if (count($parts) === 2) {
        $parts[] = '0';
    }

    if (count($parts) !== 3) {
        throw new \RuntimeException('Invalid version number: ' . $version);
    }

    $hex = '0x';

    foreach ($parts as $value) {
        if (! ctype_digit($value) || strlen($value) > 3) {
            throw new \RuntimeException('Invalid version number: ' . $version);
        }

        $value = (int) $value;

        if ($value > 255) {
            throw new \RuntimeException('Invalid version number: ' . $version);
        }

        $value = dechex($value);

        if (strlen($value) === 1) {
            $value = '0' . $value;
        }

        $hex .= $value;
    }

    return $hex;
}
