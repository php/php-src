--TEST--
Test parse_url() function: Parse a load of URLs without specifying the component 
--FILE--
<?php
/* Prototype  : proto mixed parse_url(string url, [int url_component])
 * Description: Parse a and return its components 
 * Source code: ext/standard/url.c
 * Alias to functions: 
 */

/*
 * Parse a load of URLs without specifying the component
 */
include_once(dirname(__FILE__) . '/urls.inc');

foreach ($urls as $url) {
	echo "\n--> $url: ";
	var_dump(parse_url($url));
}

echo "Done";
?>
--EXPECTF--

--> 64.246.30.37: array(1) {
  [u"path"]=>
  unicode(12) "64.246.30.37"
}

--> http://64.246.30.37: array(2) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(12) "64.246.30.37"
}

--> http://64.246.30.37/: array(3) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(12) "64.246.30.37"
  [u"path"]=>
  unicode(1) "/"
}

--> 64.246.30.37/: array(1) {
  [u"path"]=>
  unicode(13) "64.246.30.37/"
}

--> 64.246.30.37:80/: array(3) {
  [u"host"]=>
  unicode(12) "64.246.30.37"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(1) "/"
}

--> php.net: array(1) {
  [u"path"]=>
  unicode(7) "php.net"
}

--> php.net/: array(1) {
  [u"path"]=>
  unicode(8) "php.net/"
}

--> http://php.net: array(2) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(7) "php.net"
}

--> http://php.net/: array(3) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(7) "php.net"
  [u"path"]=>
  unicode(1) "/"
}

--> www.php.net: array(1) {
  [u"path"]=>
  unicode(11) "www.php.net"
}

--> www.php.net/: array(1) {
  [u"path"]=>
  unicode(12) "www.php.net/"
}

--> http://www.php.net: array(2) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
}

--> http://www.php.net/: array(3) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"path"]=>
  unicode(1) "/"
}

--> www.php.net:80: array(2) {
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
}

--> http://www.php.net:80: array(3) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
}

--> http://www.php.net:80/: array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(1) "/"
}

--> http://www.php.net/index.php: array(3) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"path"]=>
  unicode(10) "/index.php"
}

--> www.php.net/?: array(1) {
  [u"path"]=>
  unicode(12) "www.php.net/"
}

--> www.php.net:80/?: array(3) {
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(1) "/"
}

--> http://www.php.net/?: array(3) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"path"]=>
  unicode(1) "/"
}

--> http://www.php.net:80/?: array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(1) "/"
}

--> http://www.php.net:80/index.php: array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(10) "/index.php"
}

--> http://www.php.net:80/foo/bar/index.php: array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(18) "/foo/bar/index.php"
}

--> http://www.php.net:80/this/is/a/very/deep/directory/structure/and/file.php: array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(53) "/this/is/a/very/deep/directory/structure/and/file.php"
}

--> http://www.php.net:80/this/is/a/very/deep/directory/structure/and/file.php?lots=1&of=2&parameters=3&too=4&here=5: array(5) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(53) "/this/is/a/very/deep/directory/structure/and/file.php"
  [u"query"]=>
  unicode(37) "lots=1&of=2&parameters=3&too=4&here=5"
}

--> http://www.php.net:80/this/is/a/very/deep/directory/structure/and/: array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(45) "/this/is/a/very/deep/directory/structure/and/"
}

--> http://www.php.net:80/this/is/a/very/deep/directory/structure/and/file.php: array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(53) "/this/is/a/very/deep/directory/structure/and/file.php"
}

--> http://www.php.net:80/this/../a/../deep/directory: array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(28) "/this/../a/../deep/directory"
}

--> http://www.php.net:80/this/../a/../deep/directory/: array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(29) "/this/../a/../deep/directory/"
}

--> http://www.php.net:80/this/is/a/very/deep/directory/../file.php: array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(42) "/this/is/a/very/deep/directory/../file.php"
}

--> http://www.php.net:80/index.php: array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(10) "/index.php"
}

--> http://www.php.net:80/index.php?: array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(10) "/index.php"
}

--> http://www.php.net:80/#foo: array(5) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(1) "/"
  [u"fragment"]=>
  unicode(3) "foo"
}

--> http://www.php.net:80/?#: array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(1) "/"
}

--> http://www.php.net:80/?test=1: array(5) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(1) "/"
  [u"query"]=>
  unicode(6) "test=1"
}

--> http://www.php.net/?test=1&: array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"path"]=>
  unicode(1) "/"
  [u"query"]=>
  unicode(7) "test=1&"
}

--> http://www.php.net:80/?&: array(5) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(1) "/"
  [u"query"]=>
  unicode(1) "&"
}

--> http://www.php.net:80/index.php?test=1&: array(5) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(10) "/index.php"
  [u"query"]=>
  unicode(7) "test=1&"
}

--> http://www.php.net/index.php?&: array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"path"]=>
  unicode(10) "/index.php"
  [u"query"]=>
  unicode(1) "&"
}

--> http://www.php.net:80/index.php?foo&: array(5) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(10) "/index.php"
  [u"query"]=>
  unicode(4) "foo&"
}

--> http://www.php.net/index.php?&foo: array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"path"]=>
  unicode(10) "/index.php"
  [u"query"]=>
  unicode(4) "&foo"
}

--> http://www.php.net:80/index.php?test=1&test2=char&test3=mixesCI: array(5) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(10) "/index.php"
  [u"query"]=>
  unicode(31) "test=1&test2=char&test3=mixesCI"
}

--> www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123: array(5) {
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(10) "/index.php"
  [u"query"]=>
  unicode(31) "test=1&test2=char&test3=mixesCI"
  [u"fragment"]=>
  unicode(16) "some_page_ref123"
}

--> http://secret@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123: array(7) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"user"]=>
  unicode(6) "secret"
  [u"path"]=>
  unicode(10) "/index.php"
  [u"query"]=>
  unicode(31) "test=1&test2=char&test3=mixesCI"
  [u"fragment"]=>
  unicode(16) "some_page_ref123"
}

--> http://secret:@www.php.net/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123: array(6) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"user"]=>
  unicode(6) "secret"
  [u"path"]=>
  unicode(10) "/index.php"
  [u"query"]=>
  unicode(31) "test=1&test2=char&test3=mixesCI"
  [u"fragment"]=>
  unicode(16) "some_page_ref123"
}

--> http://:hideout@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123: array(7) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"pass"]=>
  unicode(7) "hideout"
  [u"path"]=>
  unicode(10) "/index.php"
  [u"query"]=>
  unicode(31) "test=1&test2=char&test3=mixesCI"
  [u"fragment"]=>
  unicode(16) "some_page_ref123"
}

--> http://secret:hideout@www.php.net/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123: array(7) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"user"]=>
  unicode(6) "secret"
  [u"pass"]=>
  unicode(7) "hideout"
  [u"path"]=>
  unicode(10) "/index.php"
  [u"query"]=>
  unicode(31) "test=1&test2=char&test3=mixesCI"
  [u"fragment"]=>
  unicode(16) "some_page_ref123"
}

--> http://secret@hideout@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123: array(7) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"user"]=>
  unicode(14) "secret@hideout"
  [u"path"]=>
  unicode(10) "/index.php"
  [u"query"]=>
  unicode(31) "test=1&test2=char&test3=mixesCI"
  [u"fragment"]=>
  unicode(16) "some_page_ref123"
}

--> http://secret:hid:out@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123: array(8) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"user"]=>
  unicode(6) "secret"
  [u"pass"]=>
  unicode(7) "hid:out"
  [u"path"]=>
  unicode(10) "/index.php"
  [u"query"]=>
  unicode(31) "test=1&test2=char&test3=mixesCI"
  [u"fragment"]=>
  unicode(16) "some_page_ref123"
}

--> nntp://news.php.net: array(2) {
  [u"scheme"]=>
  unicode(4) "nntp"
  [u"host"]=>
  unicode(12) "news.php.net"
}

--> ftp://ftp.gnu.org/gnu/glic/glibc.tar.gz: array(3) {
  [u"scheme"]=>
  unicode(3) "ftp"
  [u"host"]=>
  unicode(11) "ftp.gnu.org"
  [u"path"]=>
  unicode(22) "/gnu/glic/glibc.tar.gz"
}

--> zlib:http://foo@bar: array(2) {
  [u"scheme"]=>
  unicode(4) "zlib"
  [u"path"]=>
  unicode(14) "http://foo@bar"
}

--> zlib:filename.txt: array(2) {
  [u"scheme"]=>
  unicode(4) "zlib"
  [u"path"]=>
  unicode(12) "filename.txt"
}

--> zlib:/path/to/my/file/file.txt: array(2) {
  [u"scheme"]=>
  unicode(4) "zlib"
  [u"path"]=>
  unicode(25) "/path/to/my/file/file.txt"
}

--> foo://foo@bar: array(3) {
  [u"scheme"]=>
  unicode(3) "foo"
  [u"host"]=>
  unicode(3) "bar"
  [u"user"]=>
  unicode(3) "foo"
}

--> mailto:me@mydomain.com: array(2) {
  [u"scheme"]=>
  unicode(6) "mailto"
  [u"path"]=>
  unicode(15) "me@mydomain.com"
}

--> /foo.php?a=b&c=d: array(2) {
  [u"path"]=>
  unicode(8) "/foo.php"
  [u"query"]=>
  unicode(7) "a=b&c=d"
}

--> foo.php?a=b&c=d: array(2) {
  [u"path"]=>
  unicode(7) "foo.php"
  [u"query"]=>
  unicode(7) "a=b&c=d"
}

--> http://user:passwd@www.example.com:8080?bar=1&boom=0: array(6) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(15) "www.example.com"
  [u"port"]=>
  int(8080)
  [u"user"]=>
  unicode(4) "user"
  [u"pass"]=>
  unicode(6) "passwd"
  [u"query"]=>
  unicode(12) "bar=1&boom=0"
}

--> file:///path/to/file: array(2) {
  [u"scheme"]=>
  unicode(4) "file"
  [u"path"]=>
  unicode(13) "/path/to/file"
}

--> file://path/to/file: array(3) {
  [u"scheme"]=>
  unicode(4) "file"
  [u"host"]=>
  unicode(4) "path"
  [u"path"]=>
  unicode(8) "/to/file"
}

--> file:/path/to/file: array(2) {
  [u"scheme"]=>
  unicode(4) "file"
  [u"path"]=>
  unicode(13) "/path/to/file"
}

--> http://1.2.3.4:/abc.asp?a=1&b=2: array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(7) "1.2.3.4"
  [u"path"]=>
  unicode(8) "/abc.asp"
  [u"query"]=>
  unicode(7) "a=1&b=2"
}

--> http://foo.com#bar: array(3) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(7) "foo.com"
  [u"fragment"]=>
  unicode(3) "bar"
}

--> scheme:: array(1) {
  [u"scheme"]=>
  unicode(6) "scheme"
}

--> foo+bar://baz@bang/bla: array(4) {
  [u"scheme"]=>
  unicode(7) "foo+bar"
  [u"host"]=>
  unicode(4) "bang"
  [u"user"]=>
  unicode(3) "baz"
  [u"path"]=>
  unicode(4) "/bla"
}

--> gg:9130731: array(2) {
  [u"scheme"]=>
  unicode(2) "gg"
  [u"path"]=>
  unicode(7) "9130731"
}

--> http://user:@pass@host/path?argument?value#etc: array(7) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(4) "host"
  [u"user"]=>
  unicode(4) "user"
  [u"pass"]=>
  unicode(5) "@pass"
  [u"path"]=>
  unicode(5) "/path"
  [u"query"]=>
  unicode(14) "argument?value"
  [u"fragment"]=>
  unicode(3) "etc"
}

--> http://10.10.10.10/:80: array(3) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "10.10.10.10"
  [u"path"]=>
  unicode(4) "/:80"
}

--> http://x:?: array(2) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(1) "x"
}

--> x:blah.com: array(2) {
  [u"scheme"]=>
  unicode(1) "x"
  [u"path"]=>
  unicode(8) "blah.com"
}

--> x:/blah.com: array(2) {
  [u"scheme"]=>
  unicode(1) "x"
  [u"path"]=>
  unicode(9) "/blah.com"
}

--> x://::abc/?: array(3) {
  [u"scheme"]=>
  unicode(1) "x"
  [u"host"]=>
  unicode(1) ":"
  [u"path"]=>
  unicode(1) "/"
}

--> http://::?: array(2) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(1) ":"
}

--> x://::6.5: array(3) {
  [u"scheme"]=>
  unicode(1) "x"
  [u"host"]=>
  unicode(1) ":"
  [u"port"]=>
  int(6)
}

--> http://?:/: array(3) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(1) "?"
  [u"path"]=>
  unicode(1) "/"
}

--> http://@?:/: array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(1) "?"
  [u"user"]=>
  unicode(0) ""
  [u"path"]=>
  unicode(1) "/"
}

--> file:///:: array(2) {
  [u"scheme"]=>
  unicode(4) "file"
  [u"path"]=>
  unicode(2) "/:"
}

--> file:///a:/: array(2) {
  [u"scheme"]=>
  unicode(4) "file"
  [u"path"]=>
  unicode(3) "a:/"
}

--> file:///ab:/: array(2) {
  [u"scheme"]=>
  unicode(4) "file"
  [u"path"]=>
  unicode(5) "/ab:/"
}

--> file:///a:/: array(2) {
  [u"scheme"]=>
  unicode(4) "file"
  [u"path"]=>
  unicode(3) "a:/"
}

--> file:///@:/: array(2) {
  [u"scheme"]=>
  unicode(4) "file"
  [u"path"]=>
  unicode(3) "@:/"
}

--> file:///:80/: array(2) {
  [u"scheme"]=>
  unicode(4) "file"
  [u"path"]=>
  unicode(5) "/:80/"
}

--> []: array(1) {
  [u"path"]=>
  unicode(2) "[]"
}

--> http://[x:80]/: array(3) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(6) "[x:80]"
  [u"path"]=>
  unicode(1) "/"
}

--> : array(1) {
  [u"path"]=>
  unicode(0) ""
}

--> /: array(1) {
  [u"path"]=>
  unicode(1) "/"
}

--> http:///blah.com: 
Warning: parse_url(http:///blah.com): Unable to parse URL in %s on line 15
bool(false)

--> http://:80: 
Warning: parse_url(http://:80): Unable to parse URL in %s on line 15
bool(false)

--> http://user@:80: 
Warning: parse_url(http://user@:80): Unable to parse URL in %s on line 15
bool(false)

--> http://user:pass@:80: 
Warning: parse_url(http://user:pass@:80): Unable to parse URL in %s on line 15
bool(false)

--> http://:: 
Warning: parse_url(http://:): Unable to parse URL in %s on line 15
bool(false)

--> http://@/: 
Warning: parse_url(http://@/): Unable to parse URL in %s on line 15
bool(false)

--> http://@:/: 
Warning: parse_url(http://@:/): Unable to parse URL in %s on line 15
bool(false)

--> http://:/: 
Warning: parse_url(http://:/): Unable to parse URL in %s on line 15
bool(false)

--> http://?: 
Warning: parse_url(http://?): Unable to parse URL in %s on line 15
bool(false)

--> http://?:: 
Warning: parse_url(http://?:): Unable to parse URL in %s on line 15
bool(false)

--> http://:?: 
Warning: parse_url(http://:?): Unable to parse URL in %s on line 15
bool(false)

--> http://blah.com:123456: 
Warning: parse_url(http://blah.com:123456): Unable to parse URL in %s on line 15
bool(false)

--> http://blah.com:abcdef: 
Warning: parse_url(http://blah.com:abcdef): Unable to parse URL in %s on line 15
bool(false)
Done
