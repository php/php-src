--TEST--
Uri\WhatWg\UrlValidationErrorType uses the correct InvalidReverseSolidus spelling
--EXTENSIONS--
uri
--FILE--
<?php
$caseNames = array_map(
    static fn($case) => $case->name,
    Uri\WhatWg\UrlValidationErrorType::cases(),
);

var_dump(in_array('InvalidReverseSolidus', $caseNames, true));
var_dump(in_array('InvalidReverseSoldius', $caseNames, true));
?>
--EXPECT--
bool(true)
bool(false)
