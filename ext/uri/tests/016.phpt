--TEST--
Test property mutation errors
--EXTENSIONS--
reflection
uri
--FILE--
<?php

function mutateProperty($uri, string $name, mixed $value)
{
    try {
        $rp = new ReflectionProperty($uri, $name);
        $rp->setValue($uri, $value);
    } catch (Error $e) {
        echo $e->getMessage() . "\n";
    }
}

$uri = Uri\Rfc3986\Uri::parse("https://example.com");
mutateProperty($uri, "scheme", "");
mutateProperty($uri, "user", "");
mutateProperty($uri, "password", "");
mutateProperty($uri, "host", "");
mutateProperty($uri, "port", 1);
mutateProperty($uri, "path", "");
mutateProperty($uri, "query", "");
mutateProperty($uri, "fragment", "");

$uri = Uri\WhatWg\Url::parse("https://example.com");
mutateProperty($uri, "scheme", "");
mutateProperty($uri, "user", "");
mutateProperty($uri, "password", "");
mutateProperty($uri, "host", "");
mutateProperty($uri, "port", 1);
mutateProperty($uri, "path", "");
mutateProperty($uri, "query", "");
mutateProperty($uri, "fragment", "");

?>
--EXPECT--
Cannot modify readonly property Uri\Rfc3986\Uri::$scheme
Cannot modify readonly property Uri\Rfc3986\Uri::$user
Cannot modify readonly property Uri\Rfc3986\Uri::$password
Cannot modify readonly property Uri\Rfc3986\Uri::$host
Cannot modify readonly property Uri\Rfc3986\Uri::$port
Cannot modify readonly property Uri\Rfc3986\Uri::$path
Cannot modify readonly property Uri\Rfc3986\Uri::$query
Cannot modify readonly property Uri\Rfc3986\Uri::$fragment
Cannot modify readonly property Uri\WhatWg\Url::$scheme
Cannot modify readonly property Uri\WhatWg\Url::$user
Cannot modify readonly property Uri\WhatWg\Url::$password
Cannot modify readonly property Uri\WhatWg\Url::$host
Cannot modify readonly property Uri\WhatWg\Url::$port
Cannot modify readonly property Uri\WhatWg\Url::$path
Cannot modify readonly property Uri\WhatWg\Url::$query
Cannot modify readonly property Uri\WhatWg\Url::$fragment
