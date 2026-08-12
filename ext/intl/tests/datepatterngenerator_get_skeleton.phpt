--TEST--
IntlDatePatternGenerator::getSkeleton() and getBaseSkeleton()
--EXTENSIONS--
intl
--FILE--
<?php

var_dump(IntlDatePatternGenerator::getSkeleton("dd/MMM"));
var_dump(IntlDatePatternGenerator::getSkeleton("MMM-dd"));
var_dump(IntlDatePatternGenerator::getBaseSkeleton("dd/MMM"));
var_dump(IntlDatePatternGenerator::getBaseSkeleton("MMM-dd"));
var_dump(IntlDatePatternGenerator::getSkeleton(""));
var_dump(IntlDatePatternGenerator::getBaseSkeleton(""));

$patterns = [
    "'at' HH:mm",
    "y年M月d日",
    "yyyy-MM-dd",
    "MMMMM",
    "MMM",
    "HH:mm:ss zzz dd/MM/y",
    "'at HH:mm",
];

foreach ($patterns as $pattern) {
    printf(
        "%s => [%s] [%s]\n",
        $pattern,
        IntlDatePatternGenerator::getSkeleton($pattern),
        IntlDatePatternGenerator::getBaseSkeleton($pattern),
    );
}

?>
--EXPECT--
string(5) "MMMdd"
string(5) "MMMdd"
string(4) "MMMd"
string(4) "MMMd"
string(0) ""
string(0) ""
'at' HH:mm => [HHmm] [Hm]
y年M月d日 => [yMd] [yMd]
yyyy-MM-dd => [yyyyMMdd] [yMd]
MMMMM => [MMMMM] [MMMMM]
MMM => [MMM] [MMM]
HH:mm:ss zzz dd/MM/y => [yMMddHHmmsszzz] [yMdHmsz]
'at HH:mm => [] []
