--TEST--
Test unsetting URI properties
--EXTENSIONS--
uri
--FILE--
<?php

readonly class MyRfc3986Uri extends Uri\Rfc3986\Uri
{
    public int $foo;
}

readonly class MyWhatWgUri extends Uri\WhatWg\Url
{
    public int $foo;
}

function handleException(Uri\Uri $uri, Closure $callback)
{
    try {
        $callback($uri);
    } catch (Error $e) {
        echo $e->getMessage() . "\n";
    }
}

function unsetMembers(Uri\Uri $uri)
{
    handleException($uri, function (Uri\Uri $uri) {unset($uri->scheme);});
    handleException($uri, function (Uri\Uri $uri) {unset($uri->user);});
    handleException($uri, function (Uri\Uri $uri) {unset($uri->password);});
    handleException($uri, function (Uri\Uri $uri) {unset($uri->host);});
    handleException($uri, function (Uri\Uri $uri) {unset($uri->port);});
    handleException($uri, function (Uri\Uri $uri) {unset($uri->path);});
    handleException($uri, function (Uri\Uri $uri) {unset($uri->query);});
    handleException($uri, function (Uri\Uri $uri) {unset($uri->fragment);});
    handleException($uri, function (Uri\Uri $uri) {unset($uri->foo);});
}

unsetMembers(new MyRfc3986Uri("https://example.com"));
unsetMembers(new MyWhatWgUri("https://example.com"));

?>
--EXPECT--
Cannot unset readonly property MyRfc3986Uri::$scheme
Cannot unset readonly property MyRfc3986Uri::$user
Cannot unset readonly property MyRfc3986Uri::$password
Cannot unset readonly property MyRfc3986Uri::$host
Cannot unset readonly property MyRfc3986Uri::$port
Cannot unset readonly property MyRfc3986Uri::$path
Cannot unset readonly property MyRfc3986Uri::$query
Cannot unset readonly property MyRfc3986Uri::$fragment
Cannot unset readonly property MyRfc3986Uri::$foo
Cannot unset readonly property MyWhatWgUri::$scheme
Cannot unset readonly property MyWhatWgUri::$user
Cannot unset readonly property MyWhatWgUri::$password
Cannot unset readonly property MyWhatWgUri::$host
Cannot unset readonly property MyWhatWgUri::$port
Cannot unset readonly property MyWhatWgUri::$path
Cannot unset readonly property MyWhatWgUri::$query
Cannot unset readonly property MyWhatWgUri::$fragment
Cannot unset readonly property MyWhatWgUri::$foo
