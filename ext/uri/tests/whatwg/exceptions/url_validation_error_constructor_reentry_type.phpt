--TEST--
Test Uri\WhatWg\UrlValidationError::__construct() - error - reentry with initialized type
--FILE--
<?php

$error = unserialize(
    'O:29:"Uri\\WhatWg\\UrlValidationError":1:{s:4:"type";E:45:"Uri\\WhatWg\\UrlValidationErrorType:HostMissing";}'
);

try {
    $error->__construct('foo', Uri\WhatWg\UrlValidationErrorType::HostMissing, false);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot modify readonly property Uri\WhatWg\UrlValidationError::$type
