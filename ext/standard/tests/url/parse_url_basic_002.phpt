--TEST--
Test parse_url() function: Parse a load of URLs without specifying PHP_URL_SCHEME as the URL component
--FILE--
<?php
/*
 * Parse a load of URLs without specifying PHP_URL_SCHEME as the URL component
 */
include_once(__DIR__ . '/urls.inc');

foreach ($urls as $url) {
    echo "--> $url   : ";
    var_dump(parse_url($url, PHP_URL_SCHEME));

}

echo "Done";
?>
--EXPECT--
--> 64.246.30.37   : NULL
--> http://64.246.30.37   : string(4) "http"
--> http://64.246.30.37/   : string(4) "http"
--> 64.246.30.37/   : NULL
--> 64.246.30.37:80/   : NULL
--> php.net   : NULL
--> php.net/   : NULL
--> http://php.net   : string(4) "http"
--> http://php.net/   : string(4) "http"
--> www.php.net   : NULL
--> www.php.net/   : NULL
--> http://www.php.net   : string(4) "http"
--> http://www.php.net/   : string(4) "http"
--> www.php.net:80   : NULL
--> http://www.php.net:80   : string(4) "http"
--> http://www.php.net:80/   : string(4) "http"
--> http://www.php.net/index.php   : string(4) "http"
--> www.php.net/?   : NULL
--> www.php.net:80/?   : NULL
--> http://www.php.net/?   : string(4) "http"
--> http://www.php.net:80/?   : string(4) "http"
--> http://www.php.net:80/index.php   : string(4) "http"
--> http://www.php.net:80/foo/bar/index.php   : string(4) "http"
--> http://www.php.net:80/this/is/a/very/deep/directory/structure/and/file.php   : string(4) "http"
--> http://www.php.net:80/this/is/a/very/deep/directory/structure/and/file.php?lots=1&of=2&parameters=3&too=4&here=5   : string(4) "http"
--> http://www.php.net:80/this/is/a/very/deep/directory/structure/and/   : string(4) "http"
--> http://www.php.net:80/this/is/a/very/deep/directory/structure/and/file.php   : string(4) "http"
--> http://www.php.net:80/this/../a/../deep/directory   : string(4) "http"
--> http://www.php.net:80/this/../a/../deep/directory/   : string(4) "http"
--> http://www.php.net:80/this/is/a/very/deep/directory/../file.php   : string(4) "http"
--> http://www.php.net:80/index.php   : string(4) "http"
--> http://www.php.net:80/index.php?   : string(4) "http"
--> http://www.php.net:80/#foo   : string(4) "http"
--> http://www.php.net:80/?#   : string(4) "http"
--> http://www.php.net:80/?test=1   : string(4) "http"
--> http://www.php.net/?test=1&   : string(4) "http"
--> http://www.php.net:80/?&   : string(4) "http"
--> http://www.php.net:80/index.php?test=1&   : string(4) "http"
--> http://www.php.net/index.php?&   : string(4) "http"
--> http://www.php.net:80/index.php?foo&   : string(4) "http"
--> http://www.php.net/index.php?&foo   : string(4) "http"
--> http://www.php.net:80/index.php?test=1&test2=char&test3=mixesCI   : string(4) "http"
--> www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : NULL
--> http://secret@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : string(4) "http"
--> http://secret:@www.php.net/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : string(4) "http"
--> http://:hideout@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : string(4) "http"
--> http://secret:hideout@www.php.net/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : string(4) "http"
--> http://secret@hideout@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : string(4) "http"
--> http://secret:hid:out@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123   : string(4) "http"
--> nntp://news.php.net   : string(4) "nntp"
--> ftp://ftp.gnu.org/gnu/glic/glibc.tar.gz   : string(3) "ftp"
--> zlib:http://foo@bar   : string(4) "zlib"
--> zlib:filename.txt   : string(4) "zlib"
--> zlib:/path/to/my/file/file.txt   : string(4) "zlib"
--> foo://foo@bar   : string(3) "foo"
--> mailto:me@mydomain.com   : string(6) "mailto"
--> /foo.php?a=b&c=d   : NULL
--> foo.php?a=b&c=d   : NULL
--> http://user:passwd@www.example.com:8080?bar=1&boom=0   : string(4) "http"
--> http://user_me-you:my_pas-word@www.example.com:8080?bar=1&boom=0   : string(4) "http"
--> file:///path/to/file   : string(4) "file"
--> file://path/to/file   : string(4) "file"
--> file:/path/to/file   : string(4) "file"
--> http://1.2.3.4:/abc.asp?a=1&b=2   : string(4) "http"
--> http://foo.com#bar   : string(4) "http"
--> scheme:   : string(6) "scheme"
--> foo+bar://baz@bang/bla   : string(7) "foo+bar"
--> gg:9130731   : string(2) "gg"
--> http://user:@pass@host/path?argument?value#etc   : string(4) "http"
--> http://10.10.10.10/:80   : string(4) "http"
--> http://x:?   : string(4) "http"
--> x:blah.com   : string(1) "x"
--> x:/blah.com   : string(1) "x"
--> x://::abc/?   : bool(false)
--> http://::?   : string(4) "http"
--> http://::#   : string(4) "http"
--> x://::6.5   : string(1) "x"
--> http://?:/   : bool(false)
--> http://@?:/   : bool(false)
--> file:///:   : string(4) "file"
--> file:///a:/   : string(4) "file"
--> file:///ab:/   : string(4) "file"
--> file:///a:/   : string(4) "file"
--> file:///@:/   : string(4) "file"
--> file:///:80/   : string(4) "file"
--> []   : NULL
--> http://[x:80]/   : string(4) "http"
-->    : NULL
--> /   : NULL
--> /rest/Users?filter={"id":"123"}   : NULL
--> %:x   : NULL
--> https://example.com:0/   : string(5) "https"
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
