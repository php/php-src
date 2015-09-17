--TEST--
Test session_regenerate_id() function : basic functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

/*
 * Prototype : bool session_regenerate_id([bool $delete_old_session])
 * Description : Update the current session id with a newly generated one
 * Source code : ext/session/session.c
 */

echo "*** Testing session_regenerate_id() : basic functionality for cookie ***\n";

/* Taken from sapi/cgi/tests/include.inc */
function get_cgi_path() /* {{{ */
{
	$php = getenv("TEST_PHP_EXECUTABLE");

	$cli = false;
	$cgi = false;

	if (file_exists($php) && is_executable($php)) {
		$version = `$php -n -v`;
		if (strstr($version, "(cli)")) {
			/* that's cli */
			$cli = true;
		} else if (strpos($version, "(cgi")) {
			/* that's cgi */
			return $php;
		}
	}

	if ($cli) {
		/* trying to guess ... */
		$php_path = $php;
		for ($i = 0; $i < 2; $i++) {
			$slash_pos = strrpos($php_path, "/");
			if ($slash_pos) {
				$php_path = substr($php_path, 0, $slash_pos);
			} else {
				return FALSE;
			}
		}

		if ($php_path && is_dir($php_path) && file_exists($php_path."/cgi/php-cgi") && is_executable($php_path."/cgi/php-cgi")) {
			/* gotcha */
			return $php_path."/cgi/php-cgi";
		}
		return false;
	}
	/* uhm? what's that then? */
	return false;
}
/* }}} */

function reset_env_vars() /* {{{ */
{
	putenv("REDIRECT_STATUS");
	putenv("QUERY_STRING");
	putenv("PATH_TRANSLATED");
	putenv("SCRIPT_FILENAME");
	putenv("SERVER_SOFTWARE");
	putenv("SERVER_NAME");
	putenv("GATEWAY_INTERFACE");
	putenv("REQUEST_METHOD");
}
/* }}} */

$php = get_cgi_path();
reset_env_vars();

$file = dirname(__FILE__)."/session_regenerate_id_cookie.test.php";

file_put_contents($file, '<?php
ob_start();

function find_cookie_header() {
	$headers = headers_list();
	$target  = "Set-Cookie: PHPSESSID=";
	foreach ($headers as $h) {
		if (strstr($h, $target) !== FALSE) {
			echo $h."\n";
			return TRUE;
		}
	}
	var_dump($headers);
	return FALSE;
}

var_dump(session_start());
var_dump(find_cookie_header());
$id = session_id();
var_dump(session_regenerate_id());
var_dump(find_cookie_header());
var_dump($id !== session_id());
var_dump(session_id());
var_dump(session_destroy());

ob_end_flush();
?>');

var_dump(`$php -n -d session.name=PHPSESSID $file`);

unlink($file);

echo "Done";
?>
--EXPECTF--
*** Testing session_regenerate_id() : basic functionality for cookie ***
string(483) "X-Powered-By: PHP/7.%s
Expires: %s
Cache-Control: no-store, no-cache, must-revalidate
Pragma: no-cache
Set-Cookie: PHPSESSID=%s; path=/
Content-type: text/html; charset=UTF-8

bool(true)
Set-Cookie: PHPSESSID=%s; path=/
bool(true)
bool(true)
Set-Cookie: PHPSESSID=%s; path=/
bool(true)
bool(true)
string(32) "%s"
bool(true)
"
Done

