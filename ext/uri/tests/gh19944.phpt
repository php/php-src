--TEST--
GH-19944 (Uri\WhatWg\Url withPort boundaries validation)
--EXTENSIONS--
uri
--FILE--
<?php

try {
    $result = new \Uri\WhatWg\Url('https://example.com:432')->withPort(-1)->toAsciiString();
    echo "ERROR: Expected exception but got: " . $result . "\n";
} catch (Uri\WhatWg\InvalidUrlException $e) {
    echo "EXPECTED: " . $e->getMessage() . "\n";
    var_dump($e->errors[0]->type === \Uri\WhatWg\UrlValidationErrorType::PortOutOfRange);
}

try {
    new \Uri\WhatWg\Url('https://example.com')->withPort(65536);
    echo "ERROR: Expected exception for 65536\n";
} catch (Uri\WhatWg\InvalidUrlException $e) {
    echo "Expected exception for 65536\n";
}

try {
    $url = new \Uri\WhatWg\Url('https://example.com')->withPort(0);
    echo "Port 0 works: " . $url->getPort() . "\n";

    $url = new \Uri\WhatWg\Url('https://example.com')->withPort(65535);
    echo "Port 65535 works: " . $url->getPort() . "\n";

    $url = new \Uri\WhatWg\Url('https://example.com')->withPort(null);
    var_dump($url->getPort());
} catch (Exception $e) {
    echo "ERROR: Valid ports failed: " . $e->getMessage() . "\n";
}

?>
--EXPECT--
EXPECTED: The specified port is malformed (PortOutOfRange)
bool(true)
Expected exception for 65536
Port 0 works: 0
Port 65535 works: 65535
NULL
