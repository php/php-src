--TEST--
GH-19780 (InvalidUrlException should check $errors argument)
--EXTENSIONS--
uri
--FILE--
<?php

use Uri\WhatWg\InvalidUrlException;
use Uri\WhatWg\UrlValidationError;
use Uri\WhatWg\UrlValidationErrorType;

try {
    new InvalidUrlException('message', ['foo']);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    new InvalidUrlException('message', [
        1 => new UrlValidationError('context', UrlValidationErrorType::HostMissing, true)
    ]);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException::__construct(): Argument #2 ($errors) must be a list of Uri\WhatWg\UrlValidationError
Uri\WhatWg\InvalidUrlException::__construct(): Argument #2 ($errors) must be a list of Uri\WhatWg\UrlValidationError
