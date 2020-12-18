--TEST--
Test parse_url() function: Parse unterminated string
--SKIPIF--
<?php
if (!function_exists('zend_create_unterminated_string')) die('skip ext/test required');
?>
--FILE--
<?php

/*
 * This is the same as the basic001 test, but with unterminated strings.
 */

include_once(__DIR__ . '/urls.inc');

foreach ($urls as $url) {
    echo "\n--> $url: ";
    $str = zend_create_unterminated_string($url);
    var_dump(parse_url($str));
    zend_terminate_string($str);
}

echo "Done";
?>
--EXPECT--
--> 64.246.30.37: array(1) {
  ["path"]=>
  string(12) "64.246.30.37"
}

--> http://64.246.30.37: array(2) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(12) "64.246.30.37"
}

--> http://64.246.30.37/: array(3) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(12) "64.246.30.37"
  ["path"]=>
  string(1) "/"
}

--> 64.246.30.37/: array(1) {
  ["path"]=>
  string(13) "64.246.30.37/"
}

--> 64.246.30.37:80/: array(3) {
  ["host"]=>
  string(12) "64.246.30.37"
  ["port"]=>
  int(80)
  ["path"]=>
  string(1) "/"
}

--> php.net: array(1) {
  ["path"]=>
  string(7) "php.net"
}

--> php.net/: array(1) {
  ["path"]=>
  string(8) "php.net/"
}

--> http://php.net: array(2) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(7) "php.net"
}

--> http://php.net/: array(3) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(7) "php.net"
  ["path"]=>
  string(1) "/"
}

--> www.php.net: array(1) {
  ["path"]=>
  string(11) "www.php.net"
}

--> www.php.net/: array(1) {
  ["path"]=>
  string(12) "www.php.net/"
}

--> http://www.php.net: array(2) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
}

--> http://www.php.net/: array(3) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["path"]=>
  string(1) "/"
}

--> www.php.net:80: array(2) {
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
}

--> http://www.php.net:80: array(3) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
}

--> http://www.php.net:80/: array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(1) "/"
}

--> http://www.php.net/index.php: array(3) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["path"]=>
  string(10) "/index.php"
}

--> www.php.net/?: array(2) {
  ["path"]=>
  string(12) "www.php.net/"
  ["query"]=>
  string(0) ""
}

--> www.php.net:80/?: array(4) {
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(1) "/"
  ["query"]=>
  string(0) ""
}

--> http://www.php.net/?: array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["path"]=>
  string(1) "/"
  ["query"]=>
  string(0) ""
}

--> http://www.php.net:80/?: array(5) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(1) "/"
  ["query"]=>
  string(0) ""
}

--> http://www.php.net:80/index.php: array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(10) "/index.php"
}

--> http://www.php.net:80/foo/bar/index.php: array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(18) "/foo/bar/index.php"
}

--> http://www.php.net:80/this/is/a/very/deep/directory/structure/and/file.php: array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(53) "/this/is/a/very/deep/directory/structure/and/file.php"
}

--> http://www.php.net:80/this/is/a/very/deep/directory/structure/and/file.php?lots=1&of=2&parameters=3&too=4&here=5: array(5) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(53) "/this/is/a/very/deep/directory/structure/and/file.php"
  ["query"]=>
  string(37) "lots=1&of=2&parameters=3&too=4&here=5"
}

--> http://www.php.net:80/this/is/a/very/deep/directory/structure/and/: array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(45) "/this/is/a/very/deep/directory/structure/and/"
}

--> http://www.php.net:80/this/is/a/very/deep/directory/structure/and/file.php: array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(53) "/this/is/a/very/deep/directory/structure/and/file.php"
}

--> http://www.php.net:80/this/../a/../deep/directory: array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(28) "/this/../a/../deep/directory"
}

--> http://www.php.net:80/this/../a/../deep/directory/: array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(29) "/this/../a/../deep/directory/"
}

--> http://www.php.net:80/this/is/a/very/deep/directory/../file.php: array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(42) "/this/is/a/very/deep/directory/../file.php"
}

--> http://www.php.net:80/index.php: array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(10) "/index.php"
}

--> http://www.php.net:80/index.php?: array(5) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(10) "/index.php"
  ["query"]=>
  string(0) ""
}

--> http://www.php.net:80/#foo: array(5) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(1) "/"
  ["fragment"]=>
  string(3) "foo"
}

--> http://www.php.net:80/?#: array(6) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(1) "/"
  ["query"]=>
  string(0) ""
  ["fragment"]=>
  string(0) ""
}

--> http://www.php.net:80/?test=1: array(5) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(1) "/"
  ["query"]=>
  string(6) "test=1"
}

--> http://www.php.net/?test=1&: array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["path"]=>
  string(1) "/"
  ["query"]=>
  string(7) "test=1&"
}

--> http://www.php.net:80/?&: array(5) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(1) "/"
  ["query"]=>
  string(1) "&"
}

--> http://www.php.net:80/index.php?test=1&: array(5) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(10) "/index.php"
  ["query"]=>
  string(7) "test=1&"
}

--> http://www.php.net/index.php?&: array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["path"]=>
  string(10) "/index.php"
  ["query"]=>
  string(1) "&"
}

--> http://www.php.net:80/index.php?foo&: array(5) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(10) "/index.php"
  ["query"]=>
  string(4) "foo&"
}

--> http://www.php.net/index.php?&foo: array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["path"]=>
  string(10) "/index.php"
  ["query"]=>
  string(4) "&foo"
}

--> http://www.php.net:80/index.php?test=1&test2=char&test3=mixesCI: array(5) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(10) "/index.php"
  ["query"]=>
  string(31) "test=1&test2=char&test3=mixesCI"
}

--> www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123: array(5) {
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(10) "/index.php"
  ["query"]=>
  string(31) "test=1&test2=char&test3=mixesCI"
  ["fragment"]=>
  string(16) "some_page_ref123"
}

--> http://secret@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123: array(7) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["user"]=>
  string(6) "secret"
  ["path"]=>
  string(10) "/index.php"
  ["query"]=>
  string(31) "test=1&test2=char&test3=mixesCI"
  ["fragment"]=>
  string(16) "some_page_ref123"
}

--> http://secret:@www.php.net/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123: array(7) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["user"]=>
  string(6) "secret"
  ["pass"]=>
  string(0) ""
  ["path"]=>
  string(10) "/index.php"
  ["query"]=>
  string(31) "test=1&test2=char&test3=mixesCI"
  ["fragment"]=>
  string(16) "some_page_ref123"
}

--> http://:hideout@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123: array(8) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["user"]=>
  string(0) ""
  ["pass"]=>
  string(7) "hideout"
  ["path"]=>
  string(10) "/index.php"
  ["query"]=>
  string(31) "test=1&test2=char&test3=mixesCI"
  ["fragment"]=>
  string(16) "some_page_ref123"
}

--> http://secret:hideout@www.php.net/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123: array(7) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["user"]=>
  string(6) "secret"
  ["pass"]=>
  string(7) "hideout"
  ["path"]=>
  string(10) "/index.php"
  ["query"]=>
  string(31) "test=1&test2=char&test3=mixesCI"
  ["fragment"]=>
  string(16) "some_page_ref123"
}

--> http://secret@hideout@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123: array(7) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["user"]=>
  string(14) "secret@hideout"
  ["path"]=>
  string(10) "/index.php"
  ["query"]=>
  string(31) "test=1&test2=char&test3=mixesCI"
  ["fragment"]=>
  string(16) "some_page_ref123"
}

--> http://secret:hid:out@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123: array(8) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["user"]=>
  string(6) "secret"
  ["pass"]=>
  string(7) "hid:out"
  ["path"]=>
  string(10) "/index.php"
  ["query"]=>
  string(31) "test=1&test2=char&test3=mixesCI"
  ["fragment"]=>
  string(16) "some_page_ref123"
}

--> nntp://news.php.net: array(2) {
  ["scheme"]=>
  string(4) "nntp"
  ["host"]=>
  string(12) "news.php.net"
}

--> ftp://ftp.gnu.org/gnu/glic/glibc.tar.gz: array(3) {
  ["scheme"]=>
  string(3) "ftp"
  ["host"]=>
  string(11) "ftp.gnu.org"
  ["path"]=>
  string(22) "/gnu/glic/glibc.tar.gz"
}

--> zlib:http://foo@bar: array(2) {
  ["scheme"]=>
  string(4) "zlib"
  ["path"]=>
  string(14) "http://foo@bar"
}

--> zlib:filename.txt: array(2) {
  ["scheme"]=>
  string(4) "zlib"
  ["path"]=>
  string(12) "filename.txt"
}

--> zlib:/path/to/my/file/file.txt: array(2) {
  ["scheme"]=>
  string(4) "zlib"
  ["path"]=>
  string(25) "/path/to/my/file/file.txt"
}

--> foo://foo@bar: array(3) {
  ["scheme"]=>
  string(3) "foo"
  ["host"]=>
  string(3) "bar"
  ["user"]=>
  string(3) "foo"
}

--> mailto:me@mydomain.com: array(2) {
  ["scheme"]=>
  string(6) "mailto"
  ["path"]=>
  string(15) "me@mydomain.com"
}

--> /foo.php?a=b&c=d: array(2) {
  ["path"]=>
  string(8) "/foo.php"
  ["query"]=>
  string(7) "a=b&c=d"
}

--> foo.php?a=b&c=d: array(2) {
  ["path"]=>
  string(7) "foo.php"
  ["query"]=>
  string(7) "a=b&c=d"
}

--> http://user:passwd@www.example.com:8080?bar=1&boom=0: array(6) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(15) "www.example.com"
  ["port"]=>
  int(8080)
  ["user"]=>
  string(4) "user"
  ["pass"]=>
  string(6) "passwd"
  ["query"]=>
  string(12) "bar=1&boom=0"
}

--> http://user_me-you:my_pas-word@www.example.com:8080?bar=1&boom=0: array(6) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(15) "www.example.com"
  ["port"]=>
  int(8080)
  ["user"]=>
  string(11) "user_me-you"
  ["pass"]=>
  string(11) "my_pas-word"
  ["query"]=>
  string(12) "bar=1&boom=0"
}

--> file:///path/to/file: array(2) {
  ["scheme"]=>
  string(4) "file"
  ["path"]=>
  string(13) "/path/to/file"
}

--> file://path/to/file: array(3) {
  ["scheme"]=>
  string(4) "file"
  ["host"]=>
  string(4) "path"
  ["path"]=>
  string(8) "/to/file"
}

--> file:/path/to/file: array(2) {
  ["scheme"]=>
  string(4) "file"
  ["path"]=>
  string(13) "/path/to/file"
}

--> http://1.2.3.4:/abc.asp?a=1&b=2: array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(7) "1.2.3.4"
  ["path"]=>
  string(8) "/abc.asp"
  ["query"]=>
  string(7) "a=1&b=2"
}

--> http://foo.com#bar: array(3) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(7) "foo.com"
  ["fragment"]=>
  string(3) "bar"
}

--> scheme:: array(1) {
  ["scheme"]=>
  string(6) "scheme"
}

--> foo+bar://baz@bang/bla: array(4) {
  ["scheme"]=>
  string(7) "foo+bar"
  ["host"]=>
  string(4) "bang"
  ["user"]=>
  string(3) "baz"
  ["path"]=>
  string(4) "/bla"
}

--> gg:9130731: array(2) {
  ["scheme"]=>
  string(2) "gg"
  ["path"]=>
  string(7) "9130731"
}

--> http://user:@pass@host/path?argument?value#etc: array(7) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(4) "host"
  ["user"]=>
  string(4) "user"
  ["pass"]=>
  string(5) "@pass"
  ["path"]=>
  string(5) "/path"
  ["query"]=>
  string(14) "argument?value"
  ["fragment"]=>
  string(3) "etc"
}

--> http://10.10.10.10/:80: array(3) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "10.10.10.10"
  ["path"]=>
  string(4) "/:80"
}

--> http://x:?: array(3) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(1) "x"
  ["query"]=>
  string(0) ""
}

--> x:blah.com: array(2) {
  ["scheme"]=>
  string(1) "x"
  ["path"]=>
  string(8) "blah.com"
}

--> x:/blah.com: array(2) {
  ["scheme"]=>
  string(1) "x"
  ["path"]=>
  string(9) "/blah.com"
}

--> x://::abc/?: bool(false)

--> http://::?: array(3) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(1) ":"
  ["query"]=>
  string(0) ""
}

--> http://::#: array(3) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(1) ":"
  ["fragment"]=>
  string(0) ""
}

--> x://::6.5: array(3) {
  ["scheme"]=>
  string(1) "x"
  ["host"]=>
  string(1) ":"
  ["port"]=>
  int(6)
}

--> http://?:/: bool(false)

--> http://@?:/: bool(false)

--> file:///:: array(2) {
  ["scheme"]=>
  string(4) "file"
  ["path"]=>
  string(2) "/:"
}

--> file:///a:/: array(2) {
  ["scheme"]=>
  string(4) "file"
  ["path"]=>
  string(3) "a:/"
}

--> file:///ab:/: array(2) {
  ["scheme"]=>
  string(4) "file"
  ["path"]=>
  string(5) "/ab:/"
}

--> file:///a:/: array(2) {
  ["scheme"]=>
  string(4) "file"
  ["path"]=>
  string(3) "a:/"
}

--> file:///@:/: array(2) {
  ["scheme"]=>
  string(4) "file"
  ["path"]=>
  string(3) "@:/"
}

--> file:///:80/: array(2) {
  ["scheme"]=>
  string(4) "file"
  ["path"]=>
  string(5) "/:80/"
}

--> []: array(1) {
  ["path"]=>
  string(2) "[]"
}

--> http://[x:80]/: array(3) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(6) "[x:80]"
  ["path"]=>
  string(1) "/"
}

--> : array(1) {
  ["path"]=>
  string(0) ""
}

--> /: array(1) {
  ["path"]=>
  string(1) "/"
}

--> /rest/Users?filter={"id":"123"}: array(2) {
  ["path"]=>
  string(11) "/rest/Users"
  ["query"]=>
  string(19) "filter={"id":"123"}"
}

--> %:x: array(1) {
  ["path"]=>
  string(3) "%:x"
}

--> https://example.com:0/: array(4) {
  ["scheme"]=>
  string(5) "https"
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  int(0)
  ["path"]=>
  string(1) "/"
}

--> http:///blah.com: bool(false)

--> http://:80: bool(false)

--> http://user@:80: bool(false)

--> http://user:pass@:80: bool(false)

--> http://:: bool(false)

--> http://@/: bool(false)

--> http://@:/: bool(false)

--> http://:/: bool(false)

--> http://?: bool(false)

--> http://#: bool(false)

--> http://?:: bool(false)

--> http://:?: bool(false)

--> http://blah.com:123456: bool(false)

--> http://blah.com:abcdef: bool(false)
Done
