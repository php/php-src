<?php error_reporting(E_ALL);

/* This is based on the ucgendat.c file from the OpenLDAP project, licensed
 * as follows. */

/* Copyright 1998-2007 The OpenLDAP Foundation.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted only as authorized by the OpenLDAP
 * Public License.
 *
 * A copy of this license is available at
 * <http://www.OpenLDAP.org/license.html>.
 */

/* Copyright 2001 Computing Research Labs, New Mexico State University
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COMPUTING RESEARCH LAB OR NEW MEXICO STATE UNIVERSITY BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
 * OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

if ($argc < 2) {
    echo "Usage: php ucgendata.php ./datadir\n";
    echo "./datadir must contain:\n";
    echo "UnicodeData.txt, CaseFolding.txt, SpecialCasing.txt and DerivedCoreProperties.txt\n";
    return;
}

$dir = $argv[1];
$unicodeDataFile = $dir . '/UnicodeData.txt';
$caseFoldingFile = $dir . '/CaseFolding.txt';
$specialCasingFile = $dir . '/SpecialCasing.txt';
$derivedCorePropertiesFile = $dir . '/DerivedCoreProperties.txt';

$files = [$unicodeDataFile, $caseFoldingFile, $specialCasingFile, $derivedCorePropertiesFile];
foreach ($files as $file) {
    if (!file_exists($file)) {
        echo "File $file does not exist.\n";
        return;
    }
}

$outputFile = __DIR__ . "/../unicode_data.h";

$data = new UnicodeData;
parseUnicodeData($data, file_get_contents($unicodeDataFile));
parseCaseFolding($data, file_get_contents($caseFoldingFile));
parseSpecialCasing($data, file_get_contents($specialCasingFile));
parseDerivedCoreProperties($data, file_get_contents($derivedCorePropertiesFile));
file_put_contents($outputFile, generateData($data));

class Range {
    public $start;
    public $end;

    public function __construct(int $start, int $end) {
        $this->start = $start;
        $this->end = $end;
    }
}

class UnicodeData {
    public $propIndexes;
    public $numProps;
    public $propRanges;
    public $caseMaps;
    public $extraCaseData;

    public function __construct() {
        /*
         * List of properties expected to be found in the Unicode Character Database.
         */
        $this->propIndexes = array_flip([
            "Mn", "Mc", "Me", "Nd", "Nl", "No",
            "Zs", "Zl", "Zp", "Cc", "Cf", "Cs",
            "Co", "Cn", "Lu", "Ll", "Lt", "Lm",
            "Lo", "Pc", "Pd", "Ps", "Pe", "Po",
            "Sm", "Sc", "Sk", "So", "L", "R",
            "EN", "ES", "ET", "AN", "CS", "B",
            "S", "WS", "ON", "Pi", "Pf", "AL",
            "Cased", "Case_Ignorable"
        ]);
        $this->numProps = count($this->propIndexes);

        $this->propRanges = array_fill(0, $this->numProps, []);
        $this->caseMaps = [
            'upper' => [],
            'lower' => [],
            'title' => [],
            'fold' => [],
        ];
        $this->extraCaseData = [];
    }

    function propToIndex(string $prop) : int {
        /* Deal with directionality codes introduced in Unicode 3.0. */
        if (in_array($prop, ["BN", "NSM", "PDF", "LRE", "LRO", "RLE", "RLO", "LRI", "RLI", "FSI", "PDI"])) {
            /*
             * Mark all of these as Other Neutral to preserve compatibility with
             * older versions.
             */
            $prop = "ON";
        }

        if (!isset($this->propIndexes[$prop])) {
            throw new Exception("Unknown property $prop");
        }

        return $this->propIndexes[$prop];
    }

    public function addProp(int $code, string $prop) {
        $propIdx = self::propToIndex($prop);

        // Check if this extends the last range
        $ranges = $this->propRanges[$propIdx];
        if (!empty($ranges)) {
            $lastRange = $ranges[count($ranges) - 1];
            if ($code === $lastRange->end + 1) {
                $lastRange->end++;
                return;
            }
        }

        $this->propRanges[$propIdx][] = new Range($code, $code);
    }

    public function addPropRange(int $startCode, int $endCode, string $prop) {
        $propIdx = self::propToIndex($prop);
        $this->propRanges[$propIdx][] = new Range($startCode, $endCode);
    }

    public function addCaseMapping(string $case, int $origCode, int $mappedCode) {
        $this->caseMaps[$case][$origCode] = $mappedCode;
    }

    public function compactRangeArray(array $ranges) : array {
        // Sort by start codepoint
        usort($ranges, function (Range $r1, Range $r2) {
            return $r1->start <=> $r2->start;
        });

        $lastRange = new Range(-1, -1);
        $newRanges = [];
        foreach ($ranges as $range) {
            if ($lastRange->end == -1) {
                $lastRange = $range;
            } else if ($range->start == $lastRange->end + 1) {
                $lastRange->end = $range->end;
            } else if ($range->start > $lastRange->end + 1) {
                $newRanges[] = $lastRange;
                $lastRange = $range;
            } else {
                throw new Exception(sprintf(
                    "Overlapping ranges [%x, %x] and [%x, %x]",
                    $lastRange->start, $lastRange->end,
                    $range->start, $range->end
                ));
            }
        }
        if ($lastRange->end != -1) {
            $newRanges[] = $lastRange;
        }
        return $newRanges;
    }

    public function compactPropRanges() {
        foreach ($this->propRanges as &$ranges) {
            $ranges = $this->compactRangeArray($ranges);
        }
    }
}

function parseDataFile(string $input) {
    $lines = explode("\n", $input);
    foreach ($lines as $line) {
        // Strip comments
        if (false !== $hashPos = strpos($line, '#')) {
            $line = substr($line, 0, $hashPos);
        }

        // Skip empty lines
        $line = trim($line);
        if ($line === '') {
            continue;
        }

        $fields = array_map('trim', explode(';', $line));
        yield $fields;
    }
}

function parseUnicodeData(UnicodeData $data, string $input) : void {
    $lines = parseDataFile($input);
    foreach ($lines as $fields) {
        if (count($fields) != 15) {
            throw new Exception("Line does not contain 15 fields");
        }

        $code = intval($fields[0], 16);

        $name = $fields[1];
        if ($name === '') {
            throw new Exception("Empty name");
        }

        if ($name[0] === '<' && $name !== '<control>') {
            // This is a character range
            $lines->next();
            $nextFields = $lines->current();
            $nextCode = intval($nextFields[0], 16);

            $generalCategory = $fields[2];
            $data->addPropRange($code, $nextCode, $generalCategory);

            $bidiClass = $fields[4];
            $data->addPropRange($code, $nextCode, $bidiClass);
            continue;
        }

        $generalCategory = $fields[2];
        $data->addProp($code, $generalCategory);

        $bidiClass = $fields[4];
        $data->addProp($code, $bidiClass);

        $upperCase = intval($fields[12], 16);
        $lowerCase = intval($fields[13], 16);
        $titleCase = intval($fields[14], 16) ?: $upperCase;
        if ($upperCase) {
            $data->addCaseMapping('upper', $code, $upperCase);
        }
        if ($lowerCase) {
            $data->addCaseMapping('lower', $code, $lowerCase);
        }
        if ($titleCase) {
            $data->addCaseMapping('title', $code, $titleCase);
        }
    }
}

function parseCodes(string $strCodes) : array {
    $codes = [];
    foreach (explode(' ', $strCodes) as $strCode) {
        $codes[] = intval($strCode, 16);
    }
    return $codes;
}

function parseCaseFolding(UnicodeData $data, string $input) : void {
    foreach (parseDataFile($input) as $fields) {
        if (count($fields) != 4) {
            throw new Exception("Line does not contain 4 fields");
        }

        $code = intval($fields[0], 16);
        $status = $fields[1];
        if ($status == 'T') {
            // Use language-agnostic case folding
            continue;
        }

        if ($status == 'C' || $status == 'S') {
            $foldCode = intval($fields[2], 16);
            if (!isset($data->caseMaps['fold'][$code])) {
                $data->addCaseMapping('fold', $code, $foldCode);
            } else {
                // Add simple mapping to full mapping data
                assert(is_array($data->caseMaps['fold'][$code]));
                $data->caseMaps['fold'][$code][0] = $foldCode;
            }
        } else if ($status == 'F') {
            $foldCodes = parseCodes($fields[2]);
            $existingFoldCode = $data->caseMaps['fold'][$code] ?? $code;
            $data->caseMaps['fold'][$code] = array_merge([$code], $foldCodes);
        } else {
            assert(0);
        }
    }
}

function addSpecialCasing(UnicodeData $data, string $type, int $code, array $caseCodes) : void {
    $simpleCaseCode = $data->caseMaps[$type][$code] ?? $code;
    if (count($caseCodes) == 1) {
        if ($caseCodes[0] != $simpleCaseCode) {
            throw new Exception("Simple case code in special casing does not match");
        }

        // Special case: If a title-case character maps to itself, we may still have to store it,
        // if there is a non-trivial upper-case mapping for it
        if ($type == 'title' && $code == $caseCodes[0]
                && ($data->caseMaps['upper'][$code] ?? $code) != $code) {
            $data->caseMaps['title'][$code] = $code;
        }
        return;
    }

    if (count($caseCodes) > 3) {
        throw new Exception("Special case mapping with more than 3 code points");
    }

    $data->caseMaps[$type][$code] = array_merge([$simpleCaseCode], $caseCodes);
}

function parseSpecialCasing(UnicodeData $data, string $input) : void {
    foreach (parseDataFile($input) as $fields) {
        if (count($fields) != 5 && count($fields) != 6) {
            throw new Exception("Line does not contain 5 or 6 fields");
        }

        $code = intval($fields[0], 16);
        $lower = parseCodes($fields[1]);
        $title = parseCodes($fields[2]);
        $upper = parseCodes($fields[3]);

        $cond = $fields[4];
        if ($cond) {
            // Only use unconditional mappings
            continue;
        }

        addSpecialCasing($data, 'lower', $code, $lower);
        addSpecialCasing($data, 'upper', $code, $upper);

        // Should happen last
        addSpecialCasing($data, 'title', $code, $title);
    }
}

function parseDerivedCoreProperties(UnicodeData $data, string $input) : void {
    foreach (parseDataFile($input) as $fields) {
        if (count($fields) != 2) {
            throw new Exception("Line does not contain 2 fields");
        }

        $property = $fields[1];
        if ($property != 'Cased' && $property != 'Case_Ignorable') {
            continue;
        }

        $range = explode('..', $fields[0]);
        if (count($range) == 2) {
            $data->addPropRange(intval($range[0], 16), intval($range[1], 16), $property);
        } else if (count($range) == 1) {
            $data->addProp(intval($range[0], 16), $property);
        } else {
            throw new Exception("Invalid range");
        }
    }
}

function formatArray(array $values, int $width, string $format) : string {
    $result = '';
    $i = 0;
    $c = count($values);
    for ($i = 0; $i < $c; $i++) {
        if ($i != 0) {
            $result .= ',';
        }

        $result .= $i % $width == 0 ? "\n\t" : " ";
        $result .= sprintf($format, $values[$i]);
    }
    return $result;
}

function formatShortHexArray(array $values, int $width) : string {
    return formatArray($values, $width, "0x%04x");
}
function formatShortDecArray(array $values, int $width) : string {
    return formatArray($values, $width, "% 5d");
}
function formatIntArray(array $values, int $width) : string {
    return formatArray($values, $width, "0x%08x");
}

function generatePropData(UnicodeData $data) {
    $data->compactPropRanges();

    $propOffsets = [];
    $idx = 0;
    foreach ($data->propRanges as $ranges) {
        $num = count($ranges);
        $propOffsets[] = $num ? $idx : 0xffff;
        $idx += 2*$num;
    }

    // Add sentinel for binary search
    $propOffsets[] = $idx;

    // TODO ucgendat.c pads the prop offsets to the next multiple of 4
    // for rather debious reasons of alignment. This should probably be
    // dropped
    while (count($propOffsets) % 4 != 0) {
        $propOffsets[] = 0;
    }

    $totalRanges = $idx;

    $result = "";
    $result .= "static const unsigned short _ucprop_size = $data->numProps;\n\n";
    $result .= "static const unsigned short  _ucprop_offsets[] = {";
    $result .= formatShortHexArray($propOffsets, 8);
    $result .= "\n};\n\n";

    $values = [];
    foreach ($data->propRanges as $ranges) {
        foreach ($ranges as $range) {
            $values[] = $range->start;
            $values[] = $range->end;
        }
    }

    $result .= "static const unsigned int _ucprop_ranges[] = {";
    $result .= formatIntArray($values, 4);
    $result .= "\n};\n\n";
    return $result;
}

function flatten(array $array) {
    $result = [];
    foreach ($array as $arr) {
        foreach ($arr as $v) {
            $result[] = $v;
        }
    }
    return $result;
}

function prepareCaseData(UnicodeData $data) {
    // Don't store titlecase if it's the same as uppercase
    foreach ($data->caseMaps['title'] as $code => $titleCode) {
        if ($titleCode == ($data->caseMaps['upper'][$code] ?? $code)) {
            unset($data->caseMaps['title'][$code]);
        }
    }

    // Store full (multi-char) case mappings in a separate table and only
    // store an index into it
    foreach ($data->caseMaps as $type => $caseMap) {
        foreach ($caseMap as $code => $caseCode) {
            if (is_array($caseCode)) {
                // -1 because the first entry is the simple case mapping
                $len = count($caseCode) - 1;
                $idx = count($data->extraCaseData);
                $data->caseMaps[$type][$code] = ($len << 24) | $idx;

                foreach ($caseCode as $c) {
                    $data->extraCaseData[] = $c;
                }
            }
        }
    }
}

function generateCaseMPH(string $name, array $map) {
    $prefix = "_uccase_" . $name;
    list($gTable, $table) = generateMPH($map, $fast = false);
    echo "$name: n=", count($table), ", g=", count($gTable), "\n";

    $result = "";
    $result .= "static const unsigned {$prefix}_g_size = " . count($gTable) . ";\n";
    $result .= "static const short {$prefix}_g[] = {";
    $result .= formatShortDecArray($gTable, 8);
    $result .= "\n};\n\n";
    $result .= "static const unsigned {$prefix}_table_size = " . count($table) . ";\n";
    $result .= "static const unsigned {$prefix}_table[] = {";
    $result .= formatIntArray(flatten($table), 4);
    $result .= "\n};\n\n";
    return $result;
}

function generateCaseData(UnicodeData $data) {
    prepareCaseData($data);

    $result = "";
    $result .= generateCaseMPH('upper', $data->caseMaps['upper']);
    $result .= generateCaseMPH('lower', $data->caseMaps['lower']);
    $result .= generateCaseMPH('title', $data->caseMaps['title']);
    $result .= generateCaseMPH('fold', $data->caseMaps['fold']);
    $result .= "static const unsigned _uccase_extra_table[] = {";
    $result .= formatIntArray($data->extraCaseData, 4);
    $result .= "\n};\n\n";
    return $result;
}

function generateData(UnicodeData $data) {
    $result = <<<'HEADER'
/* This file was generated from a modified version UCData's ucgendat.
 *
 *                     DO NOT EDIT THIS FILE!
 *
 * Instead, compile ucgendat.c (bundled with PHP in ext/mbstring), download
 * the appropriate UnicodeData-x.x.x.txt and CompositionExclusions-x.x.x.txt
 * files from  http://www.unicode.org/Public/ and run this program.
 *
 * More information can be found in the UCData package. Unfortunately,
 * the project's page doesn't seem to be live anymore, so you can use
 * OpenLDAPs modified copy (look in libraries/liblunicode/ucdata) */
HEADER;
    $result .= "\n\n" . generatePropData($data);
    $result .= generateCaseData($data);

    return $result;
}

/*
 * Minimal Perfect Hash Generation
 *
 * Based on "Hash, displace, and compress" algorithm due to
 * Belazzougui, Botelho and Dietzfelbinger.
 *
 * Hash function based on https://stackoverflow.com/a/12996028/385378.
 * MPH implementation based on http://stevehanov.ca/blog/index.php?id=119.
 */

function hashInt(int $d, int $x) {
    $x ^= $d;
    $x = (($x >> 16) ^ $x) * 0x45d9f3b;
    return $x & 0xffffffff;
}

function tryGenerateMPH(array $map, int $gSize) {
    $tableSize = count($map);
    $table = [];
    $gTable = array_fill(0, $gSize, 0x7fff);
    $buckets = [];

    foreach ($map as $k => $v) {
        $h = hashInt(0, $k) % $gSize;
        $buckets[$h][] = [$k, $v];
    }

    // Sort by descending number of collisions
    usort($buckets, function ($b1, $b2) {
        return -(count($b1) <=> count($b2));
    });

    foreach ($buckets as $bucket) {
        $collisions = count($bucket);
        if ($collisions <= 1) {
            continue;
        }

        // Try values of $d until all elements placed in different slots
        $d = 1;
        $i = 0;
        $used = [];
        while ($i < $collisions) {
            if ($d > 0x7fff) {
                return [];
            }

            list($k) = $bucket[$i];
            $slot = hashInt($d, $k) % $tableSize;
            if (isset($table[$slot]) || isset($used[$slot])) {
                $d++;
                $i = 0;
                $used = [];
            } else {
                $i++;
                $used[$slot] = true;
            }
        }

        $g = hashInt(0, $bucket[0][0]) % $gSize;
        $gTable[$g] = $d;
        foreach ($bucket as $elem) {
            $table[hashInt($d, $elem[0]) % $tableSize] = $elem;
        }
    }

    $freeSlots = [];
    for ($i = 0; $i < $tableSize; $i++) {
        if (!isset($table[$i])) {
            $freeSlots[] = $i;
        }
    }

    // For buckets with only one element, we directly store the index
    $freeIdx = 0;
    foreach ($buckets as $bucket) {
        if (count($bucket) != 1) {
            continue;
        }

        $elem = $bucket[0];
        $slot = $freeSlots[$freeIdx++];
        $table[$slot] = $elem;

        $g = hashInt(0, $elem[0]) % $gSize;
        $gTable[$g] = -$slot;
    }

    ksort($gTable);
    ksort($table);

    return [$gTable, $table];
}

function generateMPH(array $map, bool $fast) {
    if ($fast) {
        // Check size starting lambda=5.0 in 0.5 increments
        for ($lambda = 5.0;; $lambda -= 0.5) {
            $m = (int) (count($map) / $lambda);
            $tmpMph = tryGenerateMPH($map, $m);
            if (!empty($tmpMph)) {
                $mph = $tmpMph;
                break;
            }
        }
    } else {
        // Check all sizes starting lambda=7.0
        $m = (int) (count($map) / 7.0);
        for (;; $m++) {
            $tmpMph = tryGenerateMPH($map, $m);
            if (!empty($tmpMph)) {
                $mph = $tmpMph;
                break;
            }
        }
    }

    return $mph;
}
