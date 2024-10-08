--TEST--
Test property mutation errors
--EXTENSIONS--
reflection
uri
--FILE--
<?php

function mutateProperty(\Uri\Uri $uri, string $name, mixed $value)
{
    try {
        $rp = new ReflectionProperty($uri, $name);
        $rp->setValue($value);
    } catch (ReflectionException $e) {
        echo $e->getMessage() . "\n";
    }
}

$uri = \Uri\Uri::fromRfc3986("https://example.com");
mutateProperty($uri, "scheme", "");
mutateProperty($uri, "user", "");
mutateProperty($uri, "password", "");
mutateProperty($uri, "host", "");
mutateProperty($uri, "port", 1);
mutateProperty($uri, "path", "");
mutateProperty($uri, "query", "");
mutateProperty($uri, "fragment", "");

$uri = \Uri\Uri::fromWhatWg("https://example.com");
mutateProperty($uri, "scheme", "");
mutateProperty($uri, "user", "");
mutateProperty($uri, "password", "");
mutateProperty($uri, "host", "");
mutateProperty($uri, "port", 1);
mutateProperty($uri, "path", "");
mutateProperty($uri, "query", "");
mutateProperty($uri, "fragment", "");

?>
--EXPECTF--
Property Uri\Rfc3986Uri::$scheme does not exist
Property Uri\Rfc3986Uri::$user does not exist
Property Uri\Rfc3986Uri::$password does not exist
Property Uri\Rfc3986Uri::$host does not exist
Property Uri\Rfc3986Uri::$port does not exist
Property Uri\Rfc3986Uri::$path does not exist
Property Uri\Rfc3986Uri::$query does not exist
Property Uri\Rfc3986Uri::$fragment does not exist
Property Uri\WhatWgUri::$scheme does not exist
Property Uri\WhatWgUri::$user does not exist
Property Uri\WhatWgUri::$password does not exist
Property Uri\WhatWgUri::$host does not exist
Property Uri\WhatWgUri::$port does not exist
Property Uri\WhatWgUri::$path does not exist
Property Uri\WhatWgUri::$query does not exist
Property Uri\WhatWgUri::$fragment does not exist
