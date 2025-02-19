--TEST--
Test instantiation without calling constructor
--EXTENSIONS--
reflection
uri
--FILE--
<?php

function handleException($uri, Closure $callback)
{
    try {
        $callback($uri);
    } catch (Uri\UninitializedUriException $e) {
        echo $e->getMessage() . "\n";
    }
}

function callMembers($uri)
{
    handleException($uri, function ($uri) {var_dump($uri);});
    handleException($uri, function ($uri) {var_export($uri); echo "\n";});
    handleException($uri, function ($uri) {$uri->getScheme();});
    if ($uri instanceof Uri\Rfc3986\Uri) {
        handleException($uri, function ($uri) {$uri->getRawScheme();});
    }
    handleException($uri, function ($uri) {$uri->getUser();});
    handleException($uri, function ($uri) {$uri->getRawUser();});
    handleException($uri, function ($uri) {$uri->getPassword();});
    handleException($uri, function ($uri) {$uri->getRawPassword();});
    handleException($uri, function ($uri) {$uri->getPort();});
    handleException($uri, function ($uri) {$uri->getPath();});
    handleException($uri, function ($uri) {$uri->getRawPath();});
    handleException($uri, function ($uri) {$uri->getQuery();});
    handleException($uri, function ($uri) {$uri->getRawQuery();});
    handleException($uri, function ($uri) {$uri->getFragment();});
    handleException($uri, function ($uri) {$uri->getRawFragment();});
    handleException($uri, function ($uri) {json_encode($uri);});
    handleException($uri, function ($uri) {$uri->__serialize();});

    if ($uri instanceof Uri\Rfc3986\Uri) {
        handleException($uri, function ($uri) {$uri->getRawHost();});
        handleException($uri, function ($uri) {$uri->getHost();});
        handleException($uri, function ($uri) {$uri->toRawString();});
        handleException($uri, function ($uri) {$uri->toString();});
    } else {
        handleException($uri, function ($uri) {$uri->getAsciiHost();});
        handleException($uri, function ($uri) {$uri->getUnicodeHost();});
        handleException($uri, function ($uri) {$uri->toAsciiString();});
        handleException($uri, function ($uri) {$uri->toUnicodeString();});
    }
}

$reflectionClass = new ReflectionClass(Uri\Rfc3986\Uri::class);
$uri = $reflectionClass->newInstanceWithoutConstructor();
callMembers($uri);

echo "\n";

$reflectionClass = new ReflectionClass(Uri\WhatWg\Url::class);
$uri = $reflectionClass->newInstanceWithoutConstructor();
callMembers($uri);

?>
--EXPECTF--
object(Uri\Rfc3986\Uri)#%d (%d) {
}
\Uri\Rfc3986\Uri::__set_state(array(
))
Uri\Rfc3986\Uri object is not correctly initialized
Uri\Rfc3986\Uri object is not correctly initialized
Uri\Rfc3986\Uri object is not correctly initialized
Uri\Rfc3986\Uri object is not correctly initialized
Uri\Rfc3986\Uri object is not correctly initialized
Uri\Rfc3986\Uri object is not correctly initialized
Uri\Rfc3986\Uri object is not correctly initialized
Uri\Rfc3986\Uri object is not correctly initialized
Uri\Rfc3986\Uri object is not correctly initialized
Uri\Rfc3986\Uri object is not correctly initialized
Uri\Rfc3986\Uri object is not correctly initialized
Uri\Rfc3986\Uri object is not correctly initialized
Uri\Rfc3986\Uri object is not correctly initialized
Uri\Rfc3986\Uri object is not correctly initialized
Uri\Rfc3986\Uri object is not correctly initialized
Uri\Rfc3986\Uri object is not correctly initialized
Uri\Rfc3986\Uri object is not correctly initialized
Uri\Rfc3986\Uri object is not correctly initialized

object(Uri\WhatWg\Url)#%d (%d) {
}
\Uri\WhatWg\Url::__set_state(array(
))
Uri\WhatWg\Url object is not correctly initialized
Uri\WhatWg\Url object is not correctly initialized
Uri\WhatWg\Url object is not correctly initialized
Uri\WhatWg\Url object is not correctly initialized
Uri\WhatWg\Url object is not correctly initialized
Uri\WhatWg\Url object is not correctly initialized
Uri\WhatWg\Url object is not correctly initialized
Uri\WhatWg\Url object is not correctly initialized
Uri\WhatWg\Url object is not correctly initialized
Uri\WhatWg\Url object is not correctly initialized
Uri\WhatWg\Url object is not correctly initialized
Uri\WhatWg\Url object is not correctly initialized
Uri\WhatWg\Url object is not correctly initialized
Uri\WhatWg\Url object is not correctly initialized
Uri\WhatWg\Url object is not correctly initialized
Uri\WhatWg\Url object is not correctly initialized
Uri\WhatWg\Url object is not correctly initialized
