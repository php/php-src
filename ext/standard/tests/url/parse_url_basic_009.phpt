--TEST--
Test parse_url() function: Parse a load of URLs without specifying PHP_URL_FRAGMENT as the URL component 
--FILE--
<?php
/* Prototype  : proto mixed parse_url(string url, [int url_component])
 * Description: Parse a URL and return its components 
 * Source code: ext/standard/url.c
 * Alias to functions: 
 */

/*
 * Parse a load of URLs without specifying PHP_URL_FRAGMENT as the URL component
 */
include_once(dirname(__FILE__) . '/urls.inc');

foreach ($urls as $url) {
	echo "--> $url   : ";
	var_dump(parse_url($url, PHP_URL_FRAGMENT));
}

echo "Done";
?>
--EXPECTF--
--> 64.246.30.37   : NULL
--> http://64.246.30.37   : NULL
--> http://64.246.30.37/   : NULL
--> 64.246.30.37/   : NULL
--> 64.246.30.37:80/   : NULL
--> php.net   : NULL
--> php.net/   : NULL
--> http://php.net   : NULL
--> http://php.net/   : NULL
--> www.php.net   : NULL
--> www.php.net/   : NULL
--> http://www.php.net   : NULL
--> http://www.php.net/   : NULL
--> www.php.net:80   : NULL
--> http://www.php.net:80   : NULL
--> http://www.php.net:80/   : NULL
--> http://www.php.net/index.php   : NULL
--> www.php.net/?   : NULL
--> www.php.net:80/?   : NULL
--> http://www.php.net/?   : NULL
--> http://www.php.net:80/?   : NULL
--> http://www.php.net:80/index.php   : NULL
--> http://www.php.net:80/foo/bar/index.php   : NULL
--> http://www.php.net:80/this/is/a/very/deep/directory/structure/and/file.php   : NULL
--> http://www.php.net:80/this/is/a/very/deep/directory/structure/and/file.php?lots=1&of=2&parameters=3&too=4&here=5   : NULL
--> http://www.php.net:80/this/is/a/very/deep/directory/structure/and/   : NULL
--> http://www.php.net:80/this/is/a/very/deep/directory/structure/and/file.php   : NULL
--> http://www.php.net:80/this/../a/../deep/directory   : NULL
--> http://www.php.net:80/this/../a/../deep/directory/   : NULL
--> http://www.php.net:80/this/is/a/very/deep/directory/../file.php   : NULL
--> http://www.php.net:80/index.php   : NULL
--> http://www.php.net:80/index.php?   : NULL
--> http://www.php.net:80/#foo   : string(3) "foo"
--> http://www.php.net:80/?#   : NULL
--> http://www.php.net:80/?test=1   : NULL
--> http://www.php.net/?test=1&   : NULL
--> http://www.php.net:80/?&   : NULL
--> http://www.php.net:80/index.php?test=1&   : NULL
--> http://www.php.net/index.php?&   : NULL
--> http://www.php.net:80/index.php?foo&   : NULL
--> http://www.php.net/index.php?&foo   : NULL
--> http://www.php.net:80/index.php?test=1&test2=char&test3=mixesCI   : NULL
--> www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : string(16) "some_page_ref123"
--> http://secret@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : string(16) "some_page_ref123"
--> http://secret:@www.php.net/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : string(16) "some_page_ref123"
--> http://:hideout@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : string(16) "some_page_ref123"
--> http://secret:hideout@www.php.net/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : string(16) "some_page_ref123"
--> http://secret@hideout@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : string(16) "some_page_ref123"
--> http://secret:hid:out@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : string(16) "some_page_ref123"
--> nntp://news.php.net   : NULL
--> ftp://ftp.gnu.org/gnu/glic/glibc.tar.gz   : NULL
--> zlib:http://foo@bar   : NULL
--> zlib:filename.txt   : NULL
--> zlib:/path/to/my/file/file.txt   : NULL
--> foo://foo@bar   : NULL
--> mailto:me@mydomain.com   : NULL
--> /foo.php?a=b&c=d   : NULL
--> foo.php?a=b&c=d   : NULL
--> http://user:passwd@www.example.com:8080?bar=1&boom=0   : NULL
--> file:///path/to/file   : NULL
--> file://path/to/file   : NULL
--> file:/path/to/file   : NULL
--> http://1.2.3.4:/abc.asp?a=1&b=2   : NULL
--> http://foo.com#bar   : string(3) "bar"
--> scheme:   : NULL
--> foo+bar://baz@bang/bla   : NULL
--> gg:9130731   : NULL
--> http://user:@pass@host/path?argument?value#etc   : string(3) "etc"
--> http://10.10.10.10/:80   : NULL
--> http://x:?   : NULL
--> x:blah.com   : NULL
--> x:/blah.com   : NULL
--> x://::abc/?   : bool(false)
--> http://::?   : NULL
--> http://::#   : NULL
--> x://::6.5   : NULL
--> http://?:/   : bool(false)
--> http://@?:/   : bool(false)
--> file:///:   : NULL
--> file:///a:/   : NULL
--> file:///ab:/   : NULL
--> file:///a:/   : NULL
--> file:///@:/   : NULL
--> file:///:80/   : NULL
--> []   : NULL
--> http://[x:80]/   : NULL
-->    : NULL
--> /   : NULL
--> /rest/Users?filter={"id":"123"}   : NULL
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
