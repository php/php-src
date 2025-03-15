--TEST--
Test cloning userland properties
--EXTENSIONS--
uri
--FILE--
<?php

readonly class MyRfc3986Uri extends Uri\Rfc3986\Uri
{
    public int $foo;

    public function setFoo(): void
    {
        $this->foo = 1;
    }

    public function __clone(): void
    {
        $this->foo = 2;
    }
}

readonly class MyWhatWgUri extends Uri\WhatWg\Url
{
    public int $foo;

    public function setFoo(): void
    {
        $this->foo = 1;
    }

    public function __clone(): void
    {
        $this->foo = 2;
    }
}

$uri1 = new MyRfc3986Uri("https://example.com");
$uri1->setFoo();
$uri2 = clone $uri1;
var_dump($uri1);
var_dump($uri2);

$uri3 = new MyWhatWgUri("https://example.com");
$uri3->setFoo();
$uri4 = clone $uri3;
var_dump($uri3);
var_dump($uri4);

?>
--EXPECTF--
object(MyRfc3986Uri)#1 (%d) {
  ["foo"]=>
  int(1)
  ["scheme"]=>
  string(5) "https"
  ["userinfo"]=>
  NULL
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  NULL
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(MyRfc3986Uri)#2 (%d) {
  ["foo"]=>
  int(2)
  ["scheme"]=>
  string(5) "https"
  ["userinfo"]=>
  NULL
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  NULL
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(MyWhatWgUri)#3 (%d) {
  ["foo"]=>
  int(1)
  ["scheme"]=>
  string(5) "https"
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(MyWhatWgUri)#4 (%d) {
  ["foo"]=>
  int(2)
  ["scheme"]=>
  string(5) "https"
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
