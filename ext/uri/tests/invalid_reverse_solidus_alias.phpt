--TEST--
Uri\WhatWg\UrlValidationErrorType keeps a deprecated alias for the InvalidReverseSoldius typo
--EXTENSIONS--
uri
--FILE--
<?php
$caseNames = array_map(
    static fn($case) => $case->name,
    Uri\WhatWg\UrlValidationErrorType::cases(),
);

// The correctly spelled case exists; the misspelled alias is not a case.
var_dump(in_array('InvalidReverseSolidus', $caseNames, true));
var_dump(in_array('InvalidReverseSoldius', $caseNames, true));

$type = Uri\WhatWg\UrlValidationErrorType::InvalidReverseSolidus;

// The deprecated alias resolves to the same case and emits a deprecation notice.
$alias = Uri\WhatWg\UrlValidationErrorType::InvalidReverseSoldius;
var_dump($alias === $type);

// Serialization uses the correct spelling and round-trips.
$serialized = serialize($type);
var_dump($serialized);
var_dump(unserialize($serialized) === $type);

$errors = [];
Uri\WhatWg\Url::parse("https:\\\\example.com", null, $errors);
var_dump(in_array(
    Uri\WhatWg\UrlValidationErrorType::InvalidReverseSolidus,
    array_map(static fn($error) => $error->type, $errors),
    true,
));
?>
--EXPECTF--
bool(true)
bool(false)

Deprecated: Constant Uri\WhatWg\UrlValidationErrorType::InvalidReverseSoldius is deprecated since 8.6, use Uri\WhatWg\UrlValidationErrorType::InvalidReverseSolidus instead in %s on line %d
bool(true)
string(63) "E:55:"Uri\WhatWg\UrlValidationErrorType:InvalidReverseSolidus";"
bool(true)
bool(true)
