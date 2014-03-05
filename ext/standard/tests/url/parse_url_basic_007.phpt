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
--> 64.246.30.37   : string(12) "64.246.30.37"
--> http://64.246.30.37   : NULL
--> http://64.246.30.37/   : string(1) "/"
--> 64.246.30.37/   : string(13) "64.246.30.37/"
--> 64.246.30.37:80/   : string(1) "/"
--> php.net   : string(7) "php.net"
--> php.net/   : string(8) "php.net/"
--> http://php.net   : NULL
--> http://php.net/   : string(1) "/"
--> www.php.net   : string(11) "www.php.net"
--> www.php.net/   : string(12) "www.php.net/"
--> http://www.php.net   : NULL
--> http://www.php.net/   : string(1) "/"
--> www.php.net:80   : NULL
--> http://www.php.net:80   : NULL
--> http://www.php.net:80/   : string(1) "/"
--> http://www.php.net/index.php   : string(10) "/index.php"
--> www.php.net/?   : string(12) "www.php.net/"
--> www.php.net:80/?   : string(1) "/"
--> http://www.php.net/?   : string(1) "/"
--> http://www.php.net:80/?   : string(1) "/"
--> http://www.php.net:80/index.php   : string(10) "/index.php"
--> http://www.php.net:80/foo/bar/index.php   : string(18) "/foo/bar/index.php"
--> http://www.php.net:80/this/is/a/very/deep/directory/structure/and/file.php   : string(53) "/this/is/a/very/deep/directory/structure/and/file.php"
--> http://www.php.net:80/this/is/a/very/deep/directory/structure/and/file.php?lots=1&of=2&parameters=3&too=4&here=5   : string(53) "/this/is/a/very/deep/directory/structure/and/file.php"
--> http://www.php.net:80/this/is/a/very/deep/directory/structure/and/   : string(45) "/this/is/a/very/deep/directory/structure/and/"
--> http://www.php.net:80/this/is/a/very/deep/directory/structure/and/file.php   : string(53) "/this/is/a/very/deep/directory/structure/and/file.php"
--> http://www.php.net:80/this/../a/../deep/directory   : string(28) "/this/../a/../deep/directory"
--> http://www.php.net:80/this/../a/../deep/directory/   : string(29) "/this/../a/../deep/directory/"
--> http://www.php.net:80/this/is/a/very/deep/directory/../file.php   : string(42) "/this/is/a/very/deep/directory/../file.php"
--> http://www.php.net:80/index.php   : string(10) "/index.php"
--> http://www.php.net:80/index.php?   : string(10) "/index.php"
--> http://www.php.net:80/#foo   : string(1) "/"
--> http://www.php.net:80/?#   : string(1) "/"
--> http://www.php.net:80/?test=1   : string(1) "/"
--> http://www.php.net/?test=1&   : string(1) "/"
--> http://www.php.net:80/?&   : string(1) "/"
--> http://www.php.net:80/index.php?test=1&   : string(10) "/index.php"
--> http://www.php.net/index.php?&   : string(10) "/index.php"
--> http://www.php.net:80/index.php?foo&   : string(10) "/index.php"
--> http://www.php.net/index.php?&foo   : string(10) "/index.php"
--> http://www.php.net:80/index.php?test=1&test2=char&test3=mixesCI   : string(10) "/index.php"
--> www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : string(10) "/index.php"
--> http://secret@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : string(10) "/index.php"
--> http://secret:@www.php.net/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : string(10) "/index.php"
--> http://:hideout@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : string(10) "/index.php"
--> http://secret:hideout@www.php.net/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : string(10) "/index.php"
--> http://secret@hideout@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : string(10) "/index.php"
--> http://secret:hid:out@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : string(10) "/index.php"
--> nntp://news.php.net   : NULL
--> ftp://ftp.gnu.org/gnu/glic/glibc.tar.gz   : string(22) "/gnu/glic/glibc.tar.gz"
--> zlib:http://foo@bar   : string(14) "http://foo@bar"
--> zlib:filename.txt   : string(12) "filename.txt"
--> zlib:/path/to/my/file/file.txt   : string(25) "/path/to/my/file/file.txt"
--> foo://foo@bar   : NULL
--> mailto:me@mydomain.com   : string(15) "me@mydomain.com"
--> /foo.php?a=b&c=d   : string(8) "/foo.php"
--> foo.php?a=b&c=d   : string(7) "foo.php"
--> http://user:passwd@www.example.com:8080?bar=1&boom=0   : NULL
--> file:///path/to/file   : string(13) "/path/to/file"
--> file://path/to/file   : string(8) "/to/file"
--> file:/path/to/file   : string(13) "/path/to/file"
--> http://1.2.3.4:/abc.asp?a=1&b=2   : string(8) "/abc.asp"
--> http://foo.com#bar   : NULL
--> scheme:   : NULL
--> foo+bar://baz@bang/bla   : string(4) "/bla"
--> gg:9130731   : string(7) "9130731"
--> http://user:@pass@host/path?argument?value#etc   : string(5) "/path"
--> http://10.10.10.10/:80   : string(4) "/:80"
--> http://x:?   : NULL
--> x:blah.com   : string(8) "blah.com"
--> x:/blah.com   : string(9) "/blah.com"
--> x://::abc/?   : bool(false)
--> http://::?   : NULL
--> http://::#   : NULL
--> x://::6.5   : NULL
--> http://?:/   : string(1) "/"
--> http://@?:/   : string(1) "/"
--> file:///:   : string(2) "/:"
--> file:///a:/   : string(3) "a:/"
--> file:///ab:/   : string(5) "/ab:/"
--> file:///a:/   : string(3) "a:/"
--> file:///@:/   : string(3) "@:/"
--> file:///:80/   : string(5) "/:80/"
--> []   : string(2) "[]"
--> http://[x:80]/   : string(1) "/"
-->    : string(0) ""
--> /   : string(1) "/"
--> http:///blah.com   : bool(false)
--> http://:80   : bool(false)
--> http://user@:80   : bool(false)
--> http://user:pass@:80   : bool(false)
--> http://:   : bool(false)
--> http://@/   : bool(false)
--> http://@:/   : bool(false)
--> http://:/   : bool(false)
--> http://?   : bool(false)
--> http://#   : bool(false)
--> http://?:   : bool(false)
--> http://:?   : bool(false)
--> http://blah.com:123456   : bool(false)
--> http://blah.com:abcdef   : bool(false)
Done
