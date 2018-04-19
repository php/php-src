<?php error_reporting(E_ALL);

$dir = __DIR__;
$unicodeDataFile = $dir . '/UnicodeData.txt';
$caseFoldingFile = $dir . '/CaseFolding.txt';
$specialCasingFile = $dir . '/SpecialCasing.txt';

$files = [$unicodeDataFile, $caseFoldingFile, $specialCasingFile];
foreach ($files as $file) {
    if (!file_exists($file)) {
        echo "File $file does not exist.\n";
        return;
    }
}

testUnicodeData(file_get_contents($unicodeDataFile));
testCaseFolding(file_get_contents($caseFoldingFile));
testSpecialCasing(file_get_contents($specialCasingFile));

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

function parseCodes(string $strCodes) : array {
    $codes = [];
    foreach (explode(' ', $strCodes) as $strCode) {
        $codes[] = intval($strCode, 16);
    }
    return $codes;
}

function testCaseMap($type, int $origCode, array $newCodes) {
    $origChar = mb_chr($origCode);
    $newStr = "";
    foreach ($newCodes as $newCode) {
        $newStr .= mb_chr($newCode);
    }

    $mbNewStr = mb_convert_case($origChar, $type);
    if ($mbNewStr !== $newStr) {
        echo "$type: $mbNewStr != $newStr\n";
    }
}

function testSimpleCaseMap($type, int $origCode, int $newCode) {
    if ($newCode) {
        testCaseMap($type, $origCode, [$newCode]);
    } else {
        testCaseMap($type, $origCode, [$origCode]);
    }
}

function testUnicodeData(string $input) {
    $uppers = [];
    $folds = [];

    foreach (parseDataFile($input) as $fields) {
        assert(count($fields) == 15);

        $code = intval($fields[0], 16);
        $upperCase = intval($fields[12], 16);
        $lowerCase = intval($fields[13], 16);
        $titleCase = intval($fields[14], 16);
        testSimpleCaseMap(MB_CASE_UPPER_SIMPLE, $code, $upperCase);
        testSimpleCaseMap(MB_CASE_LOWER_SIMPLE, $code, $lowerCase);

        // Unfortunately MB_CASE_TITLE does not actually return the title case, even when passed
        // only a single character. It does ad-hoc magic based on the character class, so that
        // certain characters, such as roman numerals or circled characters will not be
        // title-cased.
        //testSimpleCaseMap(MB_CASE_TITLE_SIMPLE, $code, $titleCase ?: $upperCase);

        $chr = mb_chr($code);
        $upper = mb_strtoupper($chr);
        $uppers[$upper][] = $chr;
        $fold = mb_convert_case($chr, 3);
        $folds[$fold][] = $chr;
    }
}

function testCaseFolding(string $input) {
    foreach (parseDataFile($input) as $fields) {
        assert(count($fields) == 4);

        $code = intval($fields[0], 16);
        $status = $fields[1];
        if ($status == 'C' || $status == 'S') {
            $foldCode = intval($fields[2], 16);
            testSimpleCaseMap(MB_CASE_FOLD_SIMPLE, $code, $foldCode);
        } else if ($status == 'F') {
            $foldCodes = parseCodes($fields[2]);
            testCaseMap(MB_CASE_FOLD, $code, $foldCodes);
        }
    }
}

function testSpecialCasing(string $input) {
    foreach (parseDataFile($input) as $fields) {
        assert(count($fields) >= 5);

        $code = intval($fields[0], 16);
        $lower = parseCodes($fields[1]);
        $title = parseCodes($fields[2]);
        $upper = parseCodes($fields[3]);

        $cond = $fields[4];
        if ($cond) {
            // We don't support conditional mappings
            continue;
        }

        testCaseMap(MB_CASE_LOWER, $code, $lower);
        testCaseMap(MB_CASE_UPPER, $code, $upper);
        testCaseMap(MB_CASE_TITLE, $code, $title);
    }
}
