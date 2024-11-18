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

$uri = Uri\Rfc3986Uri::parse("https://example.com");
mutateProperty($uri, "scheme", "");
mutateProperty($uri, "user", "");
mutateProperty($uri, "password", "");
mutateProperty($uri, "host", "");
mutateProperty($uri, "port", 1);
mutateProperty($uri, "path", "");
mutateProperty($uri, "query", "");
mutateProperty($uri, "fragment", "");

$uri = Uri\WhatWgUri::parse("https://example.com");
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
Cannot modify readonly property Uri\Rfc3986Uri::$scheme
Cannot modify readonly property Uri\Rfc3986Uri::$user
Cannot modify readonly property Uri\Rfc3986Uri::$password
Cannot modify readonly property Uri\Rfc3986Uri::$host
Cannot modify readonly property Uri\Rfc3986Uri::$port
Cannot modify readonly property Uri\Rfc3986Uri::$path
Cannot modify readonly property Uri\Rfc3986Uri::$query
Cannot modify readonly property Uri\Rfc3986Uri::$fragment
Cannot modify readonly property Uri\WhatWgUri::$scheme
Cannot modify readonly property Uri\WhatWgUri::$user
Cannot modify readonly property Uri\WhatWgUri::$password
Cannot modify readonly property Uri\WhatWgUri::$host
Cannot modify readonly property Uri\WhatWgUri::$port
Cannot modify readonly property Uri\WhatWgUri::$path
Cannot modify readonly property Uri\WhatWgUri::$query
Cannot modify readonly property Uri\WhatWgUri::$fragment
