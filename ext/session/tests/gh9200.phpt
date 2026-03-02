--TEST--
GH-9200: setcookie has an obsolete expires date format
--INI--
session.cookie_lifetime=3600
--EXTENSIONS--
session
--CGI--
--FILE--
<?php
session_name("foo");
session_id('bar');
session_start();

foreach (headers_list() as $header) {
	if (preg_match('/^Set-Cookie: foo=bar; expires=(Mon|Tue|Wed|Thu|Fri|Sat|Sun), [0-9][0-9] (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) 2[0-9][0-9][0-9] [0-9][0-9]:[0-9][0-9]:[0-9][0-9] GMT; Max-Age=3600; path=\\/$/', $header)) {
		echo "Success", PHP_EOL;
		exit;
	}
}
echo "Fail", PHP_EOL;
?>
--EXPECT--
Success
