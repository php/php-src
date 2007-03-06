--TEST--
parse_url() function
--FILE--
<?php
$sample_urls = array (
'',
'64.246.30.37',
'http://64.246.30.37',
'http://64.246.30.37/',
'64.246.30.37/',
'64.246.30.37:80/',
'php.net',
'php.net/',
'http://php.net',
'http://php.net/',
'www.php.net',
'www.php.net/',
'http://www.php.net',
'http://www.php.net/',
'www.php.net:80',
'http://www.php.net:80',
'http://www.php.net:80/',
'http://www.php.net/index.php',
'www.php.net/?',
'www.php.net:80/?',
'http://www.php.net/?',
'http://www.php.net:80/?',
'http://www.php.net:80/index.php',
'http://www.php.net:80/foo/bar/index.php',
'http://www.php.net:80/this/is/a/very/deep/directory/structure/and/file.php',
'http://www.php.net:80/this/is/a/very/deep/directory/structure/and/file.php?lots=1&of=2&parameters=3&too=4&here=5',
'http://www.php.net:80/this/is/a/very/deep/directory/structure/and/',
'http://www.php.net:80/this/is/a/very/deep/directory/structure/and/file.php',
'http://www.php.net:80/this/../a/../deep/directory',
'http://www.php.net:80/this/../a/../deep/directory/',
'http://www.php.net:80/this/is/a/very/deep/directory/../file.php',
'http://www.php.net:80/index.php',
'http://www.php.net:80/index.php?',
'http://www.php.net:80/#foo',
'http://www.php.net:80/?#',
'http://www.php.net:80/?test=1',
'http://www.php.net/?test=1&',
'http://www.php.net:80/?&',
'http://www.php.net:80/index.php?test=1&',
'http://www.php.net/index.php?&',
'http://www.php.net:80/index.php?foo&',
'http://www.php.net/index.php?&foo',
'http://www.php.net:80/index.php?test=1&test2=char&test3=mixesCI',
'www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123',
'http://secret@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123',
'http://secret:@www.php.net/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123',
'http://:hideout@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123',
'http://secret:hideout@www.php.net/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123',
'http://secret@hideout@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123',
'http://secret:hid:out@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123',
'nntp://news.php.net',
'ftp://ftp.gnu.org/gnu/glic/glibc.tar.gz',
'zlib:http://foo@bar',
'zlib:filename.txt',
'zlib:/path/to/my/file/file.txt',
'foo://foo@bar',
'mailto:me@mydomain.com',
'/foo.php?a=b&c=d',
'foo.php?a=b&c=d',
'http://user:passwd@www.example.com:8080?bar=1&boom=0',
'file:///path/to/file',
'file://path/to/file',
'file:/path/to/file',
'http://1.2.3.4:/abc.asp?a=1&b=2',
'http://foo.com#bar',
'scheme:',
'foo+bar://baz@bang/bla',
'gg:9130731',
'http://user:@pass@host/path?argument?value#etc',
);

    foreach ($sample_urls as $url) {
        var_dump(@parse_url($url));
    }

    $url = 'http://secret:hideout@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123';
    foreach (array(PHP_URL_SCHEME,PHP_URL_HOST,PHP_URL_PORT,PHP_URL_USER,PHP_URL_PASS,PHP_URL_PATH,PHP_URL_QUERY,PHP_URL_FRAGMENT) as $v) {
	var_dump(parse_url($url, $v));
    }
?>
--EXPECT--
array(1) {
  ["path"]=>
  string(0) ""
}
array(1) {
  ["path"]=>
  string(12) "64.246.30.37"
}
array(2) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(12) "64.246.30.37"
}
array(3) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(12) "64.246.30.37"
  ["path"]=>
  string(1) "/"
}
array(1) {
  ["path"]=>
  string(13) "64.246.30.37/"
}
array(3) {
  ["host"]=>
  string(12) "64.246.30.37"
  ["port"]=>
  int(80)
  ["path"]=>
  string(1) "/"
}
array(1) {
  ["path"]=>
  string(7) "php.net"
}
array(1) {
  ["path"]=>
  string(8) "php.net/"
}
array(2) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(7) "php.net"
}
array(3) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(7) "php.net"
  ["path"]=>
  string(1) "/"
}
array(1) {
  ["path"]=>
  string(11) "www.php.net"
}
array(1) {
  ["path"]=>
  string(12) "www.php.net/"
}
array(2) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
}
array(3) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["path"]=>
  string(1) "/"
}
array(2) {
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
}
array(3) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
}
array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(1) "/"
}
array(3) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["path"]=>
  string(10) "/index.php"
}
array(1) {
  ["path"]=>
  string(12) "www.php.net/"
}
array(3) {
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(1) "/"
}
array(3) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["path"]=>
  string(1) "/"
}
array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(1) "/"
}
array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(10) "/index.php"
}
array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(18) "/foo/bar/index.php"
}
array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(53) "/this/is/a/very/deep/directory/structure/and/file.php"
}
array(5) {
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
array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(45) "/this/is/a/very/deep/directory/structure/and/"
}
array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(53) "/this/is/a/very/deep/directory/structure/and/file.php"
}
array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(28) "/this/../a/../deep/directory"
}
array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(29) "/this/../a/../deep/directory/"
}
array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(42) "/this/is/a/very/deep/directory/../file.php"
}
array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(10) "/index.php"
}
array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(10) "/index.php"
}
array(5) {
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
array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["path"]=>
  string(1) "/"
}
array(5) {
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
array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["path"]=>
  string(1) "/"
  ["query"]=>
  string(7) "test=1&"
}
array(5) {
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
array(5) {
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
array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["path"]=>
  string(10) "/index.php"
  ["query"]=>
  string(1) "&"
}
array(5) {
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
array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["path"]=>
  string(10) "/index.php"
  ["query"]=>
  string(4) "&foo"
}
array(5) {
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
array(5) {
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
array(7) {
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
array(6) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["user"]=>
  string(6) "secret"
  ["path"]=>
  string(10) "/index.php"
  ["query"]=>
  string(31) "test=1&test2=char&test3=mixesCI"
  ["fragment"]=>
  string(16) "some_page_ref123"
}
array(7) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["pass"]=>
  string(7) "hideout"
  ["path"]=>
  string(10) "/index.php"
  ["query"]=>
  string(31) "test=1&test2=char&test3=mixesCI"
  ["fragment"]=>
  string(16) "some_page_ref123"
}
array(7) {
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
array(7) {
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
array(8) {
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
array(2) {
  ["scheme"]=>
  string(4) "nntp"
  ["host"]=>
  string(12) "news.php.net"
}
array(3) {
  ["scheme"]=>
  string(3) "ftp"
  ["host"]=>
  string(11) "ftp.gnu.org"
  ["path"]=>
  string(22) "/gnu/glic/glibc.tar.gz"
}
array(2) {
  ["scheme"]=>
  string(4) "zlib"
  ["path"]=>
  string(14) "http://foo@bar"
}
array(2) {
  ["scheme"]=>
  string(4) "zlib"
  ["path"]=>
  string(12) "filename.txt"
}
array(2) {
  ["scheme"]=>
  string(4) "zlib"
  ["path"]=>
  string(25) "/path/to/my/file/file.txt"
}
array(3) {
  ["scheme"]=>
  string(3) "foo"
  ["host"]=>
  string(3) "bar"
  ["user"]=>
  string(3) "foo"
}
array(2) {
  ["scheme"]=>
  string(6) "mailto"
  ["path"]=>
  string(15) "me@mydomain.com"
}
array(2) {
  ["path"]=>
  string(8) "/foo.php"
  ["query"]=>
  string(7) "a=b&c=d"
}
array(2) {
  ["path"]=>
  string(7) "foo.php"
  ["query"]=>
  string(7) "a=b&c=d"
}
array(6) {
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
array(2) {
  ["scheme"]=>
  string(4) "file"
  ["path"]=>
  string(13) "/path/to/file"
}
array(3) {
  ["scheme"]=>
  string(4) "file"
  ["host"]=>
  string(4) "path"
  ["path"]=>
  string(8) "/to/file"
}
array(2) {
  ["scheme"]=>
  string(4) "file"
  ["path"]=>
  string(13) "/path/to/file"
}
array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(7) "1.2.3.4"
  ["path"]=>
  string(8) "/abc.asp"
  ["query"]=>
  string(7) "a=1&b=2"
}
array(3) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(7) "foo.com"
  ["fragment"]=>
  string(3) "bar"
}
array(1) {
  ["scheme"]=>
  string(6) "scheme"
}
array(4) {
  ["scheme"]=>
  string(7) "foo+bar"
  ["host"]=>
  string(4) "bang"
  ["user"]=>
  string(3) "baz"
  ["path"]=>
  string(4) "/bla"
}
array(2) {
  ["scheme"]=>
  string(2) "gg"
  ["path"]=>
  string(7) "9130731"
}
array(7) {
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
string(4) "http"
string(11) "www.php.net"
int(80)
string(6) "secret"
string(7) "hideout"
string(10) "/index.php"
string(31) "test=1&test2=char&test3=mixesCI"
string(16) "some_page_ref123"
--UEXPECT--
array(1) {
  [u"path"]=>
  unicode(0) ""
}
array(1) {
  [u"path"]=>
  unicode(12) "64.246.30.37"
}
array(2) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(12) "64.246.30.37"
}
array(3) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(12) "64.246.30.37"
  [u"path"]=>
  unicode(1) "/"
}
array(1) {
  [u"path"]=>
  unicode(13) "64.246.30.37/"
}
array(3) {
  [u"host"]=>
  unicode(12) "64.246.30.37"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(1) "/"
}
array(1) {
  [u"path"]=>
  unicode(7) "php.net"
}
array(1) {
  [u"path"]=>
  unicode(8) "php.net/"
}
array(2) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(7) "php.net"
}
array(3) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(7) "php.net"
  [u"path"]=>
  unicode(1) "/"
}
array(1) {
  [u"path"]=>
  unicode(11) "www.php.net"
}
array(1) {
  [u"path"]=>
  unicode(12) "www.php.net/"
}
array(2) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
}
array(3) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"path"]=>
  unicode(1) "/"
}
array(2) {
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
}
array(3) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
}
array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(1) "/"
}
array(3) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"path"]=>
  unicode(10) "/index.php"
}
array(1) {
  [u"path"]=>
  unicode(12) "www.php.net/"
}
array(3) {
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(1) "/"
}
array(3) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"path"]=>
  unicode(1) "/"
}
array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(1) "/"
}
array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(10) "/index.php"
}
array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(18) "/foo/bar/index.php"
}
array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(53) "/this/is/a/very/deep/directory/structure/and/file.php"
}
array(5) {
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
array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(45) "/this/is/a/very/deep/directory/structure/and/"
}
array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(53) "/this/is/a/very/deep/directory/structure/and/file.php"
}
array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(28) "/this/../a/../deep/directory"
}
array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(29) "/this/../a/../deep/directory/"
}
array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(42) "/this/is/a/very/deep/directory/../file.php"
}
array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(10) "/index.php"
}
array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(10) "/index.php"
}
array(5) {
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
array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"port"]=>
  int(80)
  [u"path"]=>
  unicode(1) "/"
}
array(5) {
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
array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"path"]=>
  unicode(1) "/"
  [u"query"]=>
  unicode(7) "test=1&"
}
array(5) {
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
array(5) {
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
array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"path"]=>
  unicode(10) "/index.php"
  [u"query"]=>
  unicode(1) "&"
}
array(5) {
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
array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(11) "www.php.net"
  [u"path"]=>
  unicode(10) "/index.php"
  [u"query"]=>
  unicode(4) "&foo"
}
array(5) {
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
array(5) {
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
array(7) {
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
array(6) {
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
array(7) {
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
array(7) {
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
array(7) {
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
array(8) {
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
array(2) {
  [u"scheme"]=>
  unicode(4) "nntp"
  [u"host"]=>
  unicode(12) "news.php.net"
}
array(3) {
  [u"scheme"]=>
  unicode(3) "ftp"
  [u"host"]=>
  unicode(11) "ftp.gnu.org"
  [u"path"]=>
  unicode(22) "/gnu/glic/glibc.tar.gz"
}
array(2) {
  [u"scheme"]=>
  unicode(4) "zlib"
  [u"path"]=>
  unicode(14) "http://foo@bar"
}
array(2) {
  [u"scheme"]=>
  unicode(4) "zlib"
  [u"path"]=>
  unicode(12) "filename.txt"
}
array(2) {
  [u"scheme"]=>
  unicode(4) "zlib"
  [u"path"]=>
  unicode(25) "/path/to/my/file/file.txt"
}
array(3) {
  [u"scheme"]=>
  unicode(3) "foo"
  [u"host"]=>
  unicode(3) "bar"
  [u"user"]=>
  unicode(3) "foo"
}
array(2) {
  [u"scheme"]=>
  unicode(6) "mailto"
  [u"path"]=>
  unicode(15) "me@mydomain.com"
}
array(2) {
  [u"path"]=>
  unicode(8) "/foo.php"
  [u"query"]=>
  unicode(7) "a=b&c=d"
}
array(2) {
  [u"path"]=>
  unicode(7) "foo.php"
  [u"query"]=>
  unicode(7) "a=b&c=d"
}
array(6) {
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
array(2) {
  [u"scheme"]=>
  unicode(4) "file"
  [u"path"]=>
  unicode(13) "/path/to/file"
}
array(3) {
  [u"scheme"]=>
  unicode(4) "file"
  [u"host"]=>
  unicode(4) "path"
  [u"path"]=>
  unicode(8) "/to/file"
}
array(2) {
  [u"scheme"]=>
  unicode(4) "file"
  [u"path"]=>
  unicode(13) "/path/to/file"
}
array(4) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(7) "1.2.3.4"
  [u"path"]=>
  unicode(8) "/abc.asp"
  [u"query"]=>
  unicode(7) "a=1&b=2"
}
array(3) {
  [u"scheme"]=>
  unicode(4) "http"
  [u"host"]=>
  unicode(7) "foo.com"
  [u"fragment"]=>
  unicode(3) "bar"
}
array(1) {
  [u"scheme"]=>
  unicode(6) "scheme"
}
array(4) {
  [u"scheme"]=>
  unicode(7) "foo+bar"
  [u"host"]=>
  unicode(4) "bang"
  [u"user"]=>
  unicode(3) "baz"
  [u"path"]=>
  unicode(4) "/bla"
}
array(2) {
  [u"scheme"]=>
  unicode(2) "gg"
  [u"path"]=>
  unicode(7) "9130731"
}
array(7) {
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
unicode(4) "http"
unicode(11) "www.php.net"
int(80)
unicode(6) "secret"
unicode(7) "hideout"
unicode(10) "/index.php"
unicode(31) "test=1&test2=char&test3=mixesCI"
unicode(16) "some_page_ref123"
