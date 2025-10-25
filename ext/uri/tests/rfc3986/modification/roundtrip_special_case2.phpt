--TEST--
Test Uri\Rfc3986\Uri component modification when roundtripping is not guaranteed - case 2
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("scheme:path1:");
$uri2 = $uri1->withScheme(null);
$uri2 = $uri2->withScheme("scheme");

var_dump($uri1->toRawString());
var_dump($uri2->toRawString());
var_dump($uri2->toString());

?>
--EXPECT--
string(13) "scheme:path1:"
string(15) "scheme:./path1:"
string(13) "scheme:path1:"
