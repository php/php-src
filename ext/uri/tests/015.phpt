--TEST--
Test instantiation without calling constructor
--EXTENSIONS--
reflection
uri
--XFAIL--
URI implementations are final classes for now
--FILE--
<?php

function handleException(Uri\UriInterface $uri, Closure $callback)
{
    try {
        $callback($uri);
    } catch (Error $e) {
        echo $e->getMessage() . "\n";
    }
}

function callMembers(Uri\UriInterface $uri)
{
    handleException($uri, function (Uri\UriInterface $uri) {var_dump($uri);});
    handleException($uri, function (Uri\UriInterface $uri) {var_dump(isset($uri->scheme));});
    handleException($uri, function (Uri\UriInterface $uri) {var_dump(empty($uri->scheme));});
    handleException($uri, function (Uri\UriInterface $uri) {$uri->scheme;});
    handleException($uri, function (Uri\UriInterface $uri) {$uri->getScheme();});
    handleException($uri, function (Uri\UriInterface $uri) {$uri->user;});
    handleException($uri, function (Uri\UriInterface $uri) {$uri->getUser();});
    handleException($uri, function (Uri\UriInterface $uri) {$uri->password;});
    handleException($uri, function (Uri\UriInterface $uri) {$uri->getPassword();});
    handleException($uri, function (Uri\UriInterface $uri) {$uri->host;});
    handleException($uri, function (Uri\UriInterface $uri) {$uri->getHost();});
    handleException($uri, function (Uri\UriInterface $uri) {$uri->port;});
    handleException($uri, function (Uri\UriInterface $uri) {$uri->getPort();});
    handleException($uri, function (Uri\UriInterface $uri) {$uri->path;});
    handleException($uri, function (Uri\UriInterface $uri) {$uri->getPath();});
    handleException($uri, function (Uri\UriInterface $uri) {$uri->query;});
    handleException($uri, function (Uri\UriInterface $uri) {$uri->getQuery();});
    handleException($uri, function (Uri\UriInterface $uri) {$uri->fragment;});
    handleException($uri, function (Uri\UriInterface $uri) {$uri->getFragment();});
    handleException($uri, function (Uri\UriInterface $uri) {$uri->__toString();});
}

$reflectionClass = new ReflectionClass(Uri\Rfc3986Uri::class);
$uri = $reflectionClass->newInstanceWithoutConstructor();
callMembers($uri);

echo "\n";

$reflectionClass = new ReflectionClass(Uri\WhatWgUri::class);
$uri = $reflectionClass->newInstanceWithoutConstructor();
callMembers($uri);

?>
--EXPECTF--
object(Uri\Rfc3986Uri)#%d (%d) {
}
bool(false)
bool(true)
Uri\Rfc3986Uri object is not correctly initialized
Uri\Rfc3986Uri object is not correctly initialized
Uri\Rfc3986Uri object is not correctly initialized
Uri\Rfc3986Uri object is not correctly initialized
Uri\Rfc3986Uri object is not correctly initialized
Uri\Rfc3986Uri object is not correctly initialized
Uri\Rfc3986Uri object is not correctly initialized
Uri\Rfc3986Uri object is not correctly initialized
Uri\Rfc3986Uri object is not correctly initialized
Uri\Rfc3986Uri object is not correctly initialized
Uri\Rfc3986Uri object is not correctly initialized
Uri\Rfc3986Uri object is not correctly initialized
Uri\Rfc3986Uri object is not correctly initialized
Uri\Rfc3986Uri object is not correctly initialized
Uri\Rfc3986Uri object is not correctly initialized
Uri\Rfc3986Uri object is not correctly initialized
Uri\Rfc3986Uri object is not correctly initialized

object(Uri\WhatWgUri)#%d (%d) {
}
bool(false)
bool(true)
Uri\WhatWgUri object is not correctly initialized
Uri\WhatWgUri object is not correctly initialized
Uri\WhatWgUri object is not correctly initialized
Uri\WhatWgUri object is not correctly initialized
Uri\WhatWgUri object is not correctly initialized
Uri\WhatWgUri object is not correctly initialized
Uri\WhatWgUri object is not correctly initialized
Uri\WhatWgUri object is not correctly initialized
Uri\WhatWgUri object is not correctly initialized
Uri\WhatWgUri object is not correctly initialized
Uri\WhatWgUri object is not correctly initialized
Uri\WhatWgUri object is not correctly initialized
Uri\WhatWgUri object is not correctly initialized
Uri\WhatWgUri object is not correctly initialized
Uri\WhatWgUri object is not correctly initialized
Uri\WhatWgUri object is not correctly initialized
Uri\WhatWgUri object is not correctly initialized
