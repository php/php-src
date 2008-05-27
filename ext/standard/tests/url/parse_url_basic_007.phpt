--TEST--
Test parse_url() function: Parse a load of URLs without specifying PHP_URL_PATH as the URL component 
--FILE--
<?php
/* Prototype  : proto mixed parse_url(string url, [int url_component])
 * Description: Parse a URL and return its components 
 * Source code: ext/standard/url.c
 * Alias to functions: 
 */

/*
 * Parse a load of URLs without specifying PHP_URL_PATH as the URL component
 */
include_once(dirname(__FILE__) . '/urls.inc');

foreach ($urls as $url) {
	echo "--> $url   : ";
	var_dump(parse_url($url, PHP_URL_PATH));
}

echo "Done";
?>
--EXPECTF--
--> 64.246.30.37   : unicode(12) "64.246.30.37"
--> http://64.246.30.37   : NULL
--> http://64.246.30.37/   : unicode(1) "/"
--> 64.246.30.37/   : unicode(13) "64.246.30.37/"
--> 64.246.30.37:80/   : unicode(1) "/"
--> php.net   : unicode(7) "php.net"
--> php.net/   : unicode(8) "php.net/"
--> http://php.net   : NULL
--> http://php.net/   : unicode(1) "/"
--> www.php.net   : unicode(11) "www.php.net"
--> www.php.net/   : unicode(12) "www.php.net/"
--> http://www.php.net   : NULL
--> http://www.php.net/   : unicode(1) "/"
--> www.php.net:80   : NULL
--> http://www.php.net:80   : NULL
--> http://www.php.net:80/   : unicode(1) "/"
--> http://www.php.net/index.php   : unicode(10) "/index.php"
--> www.php.net/?   : unicode(12) "www.php.net/"
--> www.php.net:80/?   : unicode(1) "/"
--> http://www.php.net/?   : unicode(1) "/"
--> http://www.php.net:80/?   : unicode(1) "/"
--> http://www.php.net:80/index.php   : unicode(10) "/index.php"
--> http://www.php.net:80/foo/bar/index.php   : unicode(18) "/foo/bar/index.php"
--> http://www.php.net:80/this/is/a/very/deep/directory/structure/and/file.php   : unicode(53) "/this/is/a/very/deep/directory/structure/and/file.php"
--> http://www.php.net:80/this/is/a/very/deep/directory/structure/and/file.php?lots=1&of=2&parameters=3&too=4&here=5   : unicode(53) "/this/is/a/very/deep/directory/structure/and/file.php"
--> http://www.php.net:80/this/is/a/very/deep/directory/structure/and/   : unicode(45) "/this/is/a/very/deep/directory/structure/and/"
--> http://www.php.net:80/this/is/a/very/deep/directory/structure/and/file.php   : unicode(53) "/this/is/a/very/deep/directory/structure/and/file.php"
--> http://www.php.net:80/this/../a/../deep/directory   : unicode(28) "/this/../a/../deep/directory"
--> http://www.php.net:80/this/../a/../deep/directory/   : unicode(29) "/this/../a/../deep/directory/"
--> http://www.php.net:80/this/is/a/very/deep/directory/../file.php   : unicode(42) "/this/is/a/very/deep/directory/../file.php"
--> http://www.php.net:80/index.php   : unicode(10) "/index.php"
--> http://www.php.net:80/index.php?   : unicode(10) "/index.php"
--> http://www.php.net:80/#foo   : unicode(1) "/"
--> http://www.php.net:80/?#   : unicode(1) "/"
--> http://www.php.net:80/?test=1   : unicode(1) "/"
--> http://www.php.net/?test=1&   : unicode(1) "/"
--> http://www.php.net:80/?&   : unicode(1) "/"
--> http://www.php.net:80/index.php?test=1&   : unicode(10) "/index.php"
--> http://www.php.net/index.php?&   : unicode(10) "/index.php"
--> http://www.php.net:80/index.php?foo&   : unicode(10) "/index.php"
--> http://www.php.net/index.php?&foo   : unicode(10) "/index.php"
--> http://www.php.net:80/index.php?test=1&test2=char&test3=mixesCI   : unicode(10) "/index.php"
--> www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : unicode(10) "/index.php"
--> http://secret@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : unicode(10) "/index.php"
--> http://secret:@www.php.net/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : unicode(10) "/index.php"
--> http://:hideout@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : unicode(10) "/index.php"
--> http://secret:hideout@www.php.net/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : unicode(10) "/index.php"
--> http://secret@hideout@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : unicode(10) "/index.php"
--> http://secret:hid:out@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : unicode(10) "/index.php"
--> nntp://news.php.net   : NULL
--> ftp://ftp.gnu.org/gnu/glic/glibc.tar.gz   : unicode(22) "/gnu/glic/glibc.tar.gz"
--> zlib:http://foo@bar   : unicode(14) "http://foo@bar"
--> zlib:filename.txt   : unicode(12) "filename.txt"
--> zlib:/path/to/my/file/file.txt   : unicode(25) "/path/to/my/file/file.txt"
--> foo://foo@bar   : NULL
--> mailto:me@mydomain.com   : unicode(15) "me@mydomain.com"
--> /foo.php?a=b&c=d   : unicode(8) "/foo.php"
--> foo.php?a=b&c=d   : unicode(7) "foo.php"
--> http://user:passwd@www.example.com:8080?bar=1&boom=0   : NULL
--> file:///path/to/file   : unicode(13) "/path/to/file"
--> file://path/to/file   : unicode(8) "/to/file"
--> file:/path/to/file   : unicode(13) "/path/to/file"
--> http://1.2.3.4:/abc.asp?a=1&b=2   : unicode(8) "/abc.asp"
--> http://foo.com#bar   : NULL
--> scheme:   : NULL
--> foo+bar://baz@bang/bla   : unicode(4) "/bla"
--> gg:9130731   : unicode(7) "9130731"
--> http://user:@pass@host/path?argument?value#etc   : unicode(5) "/path"
--> http://10.10.10.10/:80   : unicode(4) "/:80"
--> http://x:?   : NULL
--> x:blah.com   : unicode(8) "blah.com"
--> x:/blah.com   : unicode(9) "/blah.com"
--> x://::abc/?   : unicode(1) "/"
--> http://::?   : NULL
--> x://::6.5   : NULL
--> http://?:/   : unicode(1) "/"
--> http://@?:/   : unicode(1) "/"
--> file:///:   : unicode(2) "/:"
--> file:///a:/   : unicode(3) "a:/"
--> file:///ab:/   : unicode(5) "/ab:/"
--> file:///a:/   : unicode(3) "a:/"
--> file:///@:/   : unicode(3) "@:/"
--> file:///:80/   : unicode(5) "/:80/"
--> []   : unicode(2) "[]"
--> http://[x:80]/   : unicode(1) "/"
-->    : unicode(0) ""
--> /   : unicode(1) "/"
--> http:///blah.com   : 
Warning: parse_url(http:///blah.com): Unable to parse URL in %s on line 15
bool(false)
--> http://:80   : 
Warning: parse_url(http://:80): Unable to parse URL in %s on line 15
bool(false)
--> http://user@:80   : 
Warning: parse_url(http://user@:80): Unable to parse URL in %s on line 15
bool(false)
--> http://user:pass@:80   : 
Warning: parse_url(http://user:pass@:80): Unable to parse URL in %s on line 15
bool(false)
--> http://:   : 
Warning: parse_url(http://:): Unable to parse URL in %s on line 15
bool(false)
--> http://@/   : 
Warning: parse_url(http://@/): Unable to parse URL in %s on line 15
bool(false)
--> http://@:/   : 
Warning: parse_url(http://@:/): Unable to parse URL in %s on line 15
bool(false)
--> http://:/   : 
Warning: parse_url(http://:/): Unable to parse URL in %s on line 15
bool(false)
--> http://?   : 
Warning: parse_url(http://?): Unable to parse URL in %s on line 15
bool(false)
--> http://?:   : 
Warning: parse_url(http://?:): Unable to parse URL in %s on line 15
bool(false)
--> http://:?   : 
Warning: parse_url(http://:?): Unable to parse URL in %s on line 15
bool(false)
--> http://blah.com:123456   : 
Warning: parse_url(http://blah.com:123456): Unable to parse URL in %s on line 15
bool(false)
--> http://blah.com:abcdef   : 
Warning: parse_url(http://blah.com:abcdef): Unable to parse URL in %s on line 15
bool(false)
Done
