--TEST--
Test instantiation without calling constructor
--EXTENSIONS--
reflection
uri
--FILE--
<?php

function handleException(Uri\Uri $uri, Closure $callback)
{
    try {
        $callback($uri);
    } catch (Uri\UninitializedUriException $e) {
        echo $e->getMessage() . "\n";
    }
}

function callMembers(Uri\Uri $uri)
{
    handleException($uri, function (Uri\Uri $uri) {var_dump($uri);});
    handleException($uri, function (Uri\Uri $uri) {var_dump(isset($uri->scheme));});
    handleException($uri, function (Uri\Uri $uri) {var_dump(empty($uri->scheme));});
    handleException($uri, function (Uri\Uri $uri) {$uri->scheme;});
    handleException($uri, function (Uri\Uri $uri) {$uri->getScheme();});
    handleException($uri, function (Uri\Uri $uri) {$uri->user;});
    handleException($uri, function (Uri\Uri $uri) {$uri->getUser();});
    handleException($uri, function (Uri\Uri $uri) {$uri->password;});
    handleException($uri, function (Uri\Uri $uri) {$uri->getPassword();});
    handleException($uri, function (Uri\Uri $uri) {$uri->host;});
    handleException($uri, function (Uri\Uri $uri) {$uri->getHost();});
    handleException($uri, function (Uri\Uri $uri) {$uri->port;});
    handleException($uri, function (Uri\Uri $uri) {$uri->getPort();});
    handleException($uri, function (Uri\Uri $uri) {$uri->path;});
    handleException($uri, function (Uri\Uri $uri) {$uri->getPath();});
    handleException($uri, function (Uri\Uri $uri) {$uri->query;});
    handleException($uri, function (Uri\Uri $uri) {$uri->getQuery();});
    handleException($uri, function (Uri\Uri $uri) {$uri->fragment;});
    handleException($uri, function (Uri\Uri $uri) {$uri->getFragment();});
    handleException($uri, function (Uri\Uri $uri) {json_encode($uri);});
    handleException($uri, function (Uri\Uri $uri) {$uri->__serialize();});
    handleException($uri, function (Uri\Uri $uri) {$uri->toString();});
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
bool(false)
bool(true)
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
Uri\Rfc3986\Uri object is not correctly initialized

object(Uri\WhatWg\Url)#%d (%d) {
}
bool(false)
bool(true)
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
Uri\WhatWg\Url object is not correctly initialized
Uri\WhatWg\Url object is not correctly initialized
