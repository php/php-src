--TEST--
Test session_regenerate_id() function : basic functionality
--EXTENSIONS--
session
--SKIPIF--
<?php

include('skipif.inc');

require __DIR__.'/../../../sapi/cgi/tests/include.inc';

get_cgi_path() or die('skip no cgi');

?>
--FILE--
<?php

echo "*** Testing session_regenerate_id() : basic functionality for cookie ***\n";

require __DIR__.'/../../../sapi/cgi/tests/include.inc';

$php = get_cgi_path();
reset_env_vars();

$file = __DIR__."/session_regenerate_id_cookie.test.php";

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
string(%d) "X-Powered-By: PHP/%d.%d.%s
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
