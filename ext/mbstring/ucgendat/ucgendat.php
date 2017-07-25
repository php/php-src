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

if ($argc != 2) {
    echo "Usage: php ucgendata.php UnicodeData.txt\n";
    return;
}

$inputFile = $argv[1];
if (!file_exists($inputFile)) {
    echo "File $inputFile does not exist.\n";
    return;
}

$outputFile = __DIR__ . "/../unicode_data.h";

$data = parseUnicodeData(file_get_contents($inputFile));
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
    const TO_UPPER = 0;
    const TO_LOWER = 1;
    const TO_TITLE = 2;

    public $propIndexes;
    public $numProps;
    public $propRanges;
    public $caseMaps;

    public function __construct() {
        /*
         * List of properties expected to be found in the Unicode Character Database
         * including some implementation specific properties.
         *
         * The implementation specific properties are:
         * Cm = Composed (can be decomposed)
         * Nb = Non-breaking
         * Sy = Symmetric (has left and right forms)
         * Hd = Hex digit
         * Qm = Quote marks
         * Mr = Mirroring
         * Ss = Space, other
         * Cp = Defined character
         */
        $this->propIndexes = array_flip([
            "Mn", "Mc", "Me", "Nd", "Nl", "No",
            "Zs", "Zl", "Zp", "Cc", "Cf", "Cs",
            "Co", "Cn", "Lu", "Ll", "Lt", "Lm",
            "Lo", "Pc", "Pd", "Ps", "Pe", "Po",
            "Sm", "Sc", "Sk", "So", "L", "R",
            "EN", "ES", "ET", "AN", "CS", "B",
            "S", "WS", "ON",
            "Cm", "Nb", "Sy", "Hd", "Qm", "Mr",
            "Ss", "Cp", "Pi", "Pf", "AL"
        ]);
        $this->numProps = count($this->propIndexes);

        $this->propRanges = array_fill(0, $this->numProps, []);
        $this->caseMaps = [
            self::TO_UPPER => [],
            self::TO_LOWER => [],
            self::TO_TITLE => [],
        ];
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

    public function addCaseMapping(int $case, int $origCode, int $mappedCode) {
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

function parseUnicodeData($input) {
    $data = new UnicodeData;
    $lines = array_map('trim', explode("\n", $input));
    $numLines = count($lines);
    for ($i = 0; $i < $numLines; $i++) {
        $line = $lines[$i];

        // Skip empty lines and comments
        if ($line === '' || $line[0] === '#') {
            continue;
        }

        $fields = explode(';', $line);
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
            $nextLine = $lines[$i + 1];
            $nextFields = explode(';', $nextLine);
            $nextCode = intval($nextFields[0], 16);

            $generalCategory = $fields[2];
            $data->addPropRange($code, $nextCode, $generalCategory);

            $bidiClass = $fields[4];
            $data->addPropRange($code, $nextCode, $bidiClass);

            // Excluding surrogates and private use area, mark as defined
            if ($code !== 0xd800 && $code !== 0xe000 && $code != 0xf0000) {
                $data->addPropRange($code, $nextCode, "Cp");
            }

            $i++;
            continue;
        }

        /* Add the code to the defined category. */
        $data->addProp($code, "Cp");

        $generalCategory = $fields[2];
        $data->addProp($code, $generalCategory);

        $bidiClass = $fields[4];
        $data->addProp($code, $bidiClass);

        $composition = $fields[5];
        if ($composition && $composition[0] != '<') {
            $data->addProp($code, "Cm");
        }

        $upperCase = intval($fields[12], 16);
        $lowerCase = intval($fields[13], 16);
        $titleCase = intval($fields[14], 16);
        if ($upperCase) {
            $data->addCaseMapping(UnicodeData::TO_UPPER, $code, $upperCase);
        }
        if ($lowerCase) {
            $data->addCaseMapping(UnicodeData::TO_LOWER, $code, $lowerCase);
        }
        if ($titleCase) {
            $data->addCaseMapping(UnicodeData::TO_TITLE, $code, $titleCase);
        }
    }

    return $data;
}

function formatArray(array $values, int $width, int $hexWidth) : string {
    $result = '';
    $i = 0;
    $c = count($values);
    for ($i = 0; $i < $c; $i++) {
        if ($i != 0) {
            $result .= ',';
        }

        $result .= $i % $width == 0 ? "\n\t" : " ";
        $result .= sprintf("0x%0" . $hexWidth . "x", $values[$i]);
    }
    return $result;
}

function formatShortARray(array $values, int $width) : string {
    return formatArray($values, $width, 4);
}
function formatIntArray(array $values, int $width) : string {
    return formatArray($values, $width, 8);
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
    $result .= formatShortArray($propOffsets, 8);
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

function generateCaseData(UnicodeData $data) {
    $numUpper = count($data->caseMaps[UnicodeData::TO_UPPER]);
    $numLower = count($data->caseMaps[UnicodeData::TO_LOWER]); 
    $numTitle = count($data->caseMaps[UnicodeData::TO_TITLE]); 

    $result = "";
    $result .= sprintf("static const unsigned int _uccase_size = %d;\n\n",
        $numUpper + $numLower + $numTitle);

    $result .= <<<'HEADER'
/* Starting indexes of the case tables
 * UpperIndex = 0
 * LowerIndex = _uccase_len[0]
 * TitleIndex = LowerIndex + _uccase_len[1] */
HEADER;
    $result .= "\n\n";
    $result .= sprintf("static const unsigned short _uccase_len[2] = {%d, %d};\n\n",
        $numUpper, $numLower);

    $values = [];
    foreach ($data->caseMaps as $map) {
        foreach ($map as $orig => $mapped) {
            $values[] = $orig;
            $values[] = $mapped;
        }
    }

    $result .= "static const unsigned int _uccase_map[] = {";
    $result .= formatIntArray($values, 2);
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
