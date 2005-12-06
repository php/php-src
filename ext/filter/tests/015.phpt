--TEST--
filter_data() and FL_URL
--GET--

--FILE--
<?php

$values = Array(
'http://example.com/index.html',	
'http://www.example.com/index.php',	
'http://www.example/img/test.png',	
'http://www.example/img/dir/',	
'http://www.example/img/dir',	
'http//www.example/wrong/url/',	
'http:/www.example',	
'file:///tmp/test.c',	
'ftp://ftp.example.com/tmp/',	
'/tmp/test.c',	
'/',	
'http://',	
'http:/',	
'http:',	
'http',	
'',	
-1,	
array(),	
);
foreach ($values as $value) {
	var_dump(filter_data($value, FL_URL));
}


var_dump(filter_data("qwe", FL_URL, FILTER_FLAG_SCHEME_REQUIRED));
var_dump(filter_data("http://qwe", FL_URL, FILTER_FLAG_SCHEME_REQUIRED));
var_dump(filter_data("http://", FL_URL, FILTER_FLAG_HOST_REQUIRED));
var_dump(filter_data("/tmp/test", FL_URL, FILTER_FLAG_HOST_REQUIRED));
var_dump(filter_data("http://www.example.com", FL_URL, FILTER_FLAG_HOST_REQUIRED));
var_dump(filter_data("http://www.example.com", FL_URL, FILTER_FLAG_PATH_REQUIRED));
var_dump(filter_data("http://www.example.com/path/at/the/server/", FL_URL, FILTER_FLAG_PATH_REQUIRED));
var_dump(filter_data("http://www.example.com/index.html", FL_URL, FILTER_FLAG_QUERY_REQUIRED));
var_dump(filter_data("http://www.example.com/index.php?a=b&c=d", FL_URL, FILTER_FLAG_QUERY_REQUIRED));

echo "Done\n";
?>
--EXPECTF--	
string(29) "http://example.com/index.html"
string(32) "http://www.example.com/index.php"
string(31) "http://www.example/img/test.png"
string(27) "http://www.example/img/dir/"
string(26) "http://www.example/img/dir"
string(28) "http//www.example/wrong/url/"
string(17) "http:/www.example"
string(18) "file:///tmp/test.c"
string(26) "ftp://ftp.example.com/tmp/"
string(11) "/tmp/test.c"
string(1) "/"
NULL
string(6) "http:/"
string(5) "http:"
string(4) "http"
string(0) ""
string(2) "-1"
array(0) {
}
NULL
string(10) "http://qwe"
NULL
NULL
string(22) "http://www.example.com"
NULL
string(42) "http://www.example.com/path/at/the/server/"
NULL
string(40) "http://www.example.com/index.php?a=b&c=d"
Done
