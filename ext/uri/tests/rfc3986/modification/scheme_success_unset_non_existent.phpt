--TEST--
Test Uri\Rfc3986\Uri component modification - scheme - unsetting non-existent
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("/foo/bar");
$uri2 = $uri1->withScheme(null);

var_dump($uri1->getRawScheme());
var_dump($uri2->getRawScheme());
var_dump($uri2->getScheme());

?>
--EXPECT--
NULL
NULL
NULL
