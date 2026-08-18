--TEST--
GH-19780 (InvalidUrlException should check $errors argument)
--FILE--
<?php

use Uri\WhatWg\InvalidUrlException;
use Uri\WhatWg\UrlValidationError;
use Uri\WhatWg\UrlValidationErrorType;

$error = new UrlValidationError('context', UrlValidationErrorType::HostMissing, true);

$cases = [
    'empty list' => [
        'errors' => [],
    ],
    'single validation error' => [
        'errors' => [$error],
    ],
    'multiple validation errors' => [
        'errors' => [
            $error,
            new UrlValidationError('other', UrlValidationErrorType::InvalidUrlUnit, false),
        ],
    ],
    'stdClass' => [
        'errors' => [new stdClass()],
    ],
    '[]' => [
        'errors' => [[]],
    ],
    'non-list' => [
        'errors' => [
            99 => $error,
        ],
    ],
];

foreach ($cases as $label => $case) {
    try {
        $e = new InvalidUrlException('message', $case['errors']);
        echo $label, ': ok: ', count($e->errors), "\n";
    } catch (Throwable $e) {
        echo $label, ': ', $e::class, ': ', $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
empty list: ok: 0
single validation error: ok: 1
multiple validation errors: ok: 2
stdClass: ValueError: Uri\WhatWg\InvalidUrlException::__construct(): Argument #2 ($errors) must be a list of Uri\WhatWg\UrlValidationError
[]: ValueError: Uri\WhatWg\InvalidUrlException::__construct(): Argument #2 ($errors) must be a list of Uri\WhatWg\UrlValidationError
non-list: ValueError: Uri\WhatWg\InvalidUrlException::__construct(): Argument #2 ($errors) must be a list of Uri\WhatWg\UrlValidationError
