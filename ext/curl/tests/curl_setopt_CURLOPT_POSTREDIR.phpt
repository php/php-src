--TEST--
Curl option CURLOPT_POSTREDIR
--DESCRIPTION--
Verify that CURLOPT_POSTREDIR controls whether POST data is retained on
301, 302, and 303 redirects.  By default libcurl turns POST into GET on
301, 302, and 303.  Setting the appropriate bit in CURLOPT_POSTREDIR
keeps the POST method and body.
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();

function do_redirect($code, $postredir_value = null) {
    global $host;

    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, "{$host}/get.inc?test=redirect&code={$code}");
    curl_setopt($ch, CURLOPT_POST, true);
    curl_setopt($ch, CURLOPT_POSTFIELDS, 'foo=bar');
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
    if ($postredir_value !== null) {
        curl_setopt($ch, CURLOPT_POSTREDIR, $postredir_value);
    }
    return trim(curl_exec($ch));
}

echo "default 301: ";
echo do_redirect(301), PHP_EOL;

echo "default 302: ";
echo do_redirect(302), PHP_EOL;

echo "default 303: ";
echo do_redirect(303), PHP_EOL;

echo "301 on, code 301: ";
echo do_redirect(301, CURL_REDIR_POST_301), PHP_EOL;

echo "301 on, code 302: ";
echo do_redirect(302, CURL_REDIR_POST_301), PHP_EOL;

echo "301 on, code 303: ";
echo do_redirect(303, CURL_REDIR_POST_301), PHP_EOL;

echo "302 on, code 301: ";
echo do_redirect(301, CURL_REDIR_POST_302), PHP_EOL;

echo "302 on, code 302: ";
echo do_redirect(302, CURL_REDIR_POST_302), PHP_EOL;

echo "302 on, code 303: ";
echo do_redirect(303, CURL_REDIR_POST_302), PHP_EOL;

echo "303 on, code 301: ";
echo do_redirect(301, CURL_REDIR_POST_303), PHP_EOL;

echo "303 on, code 302: ";
echo do_redirect(302, CURL_REDIR_POST_303), PHP_EOL;

echo "303 on, code 303: ";
echo do_redirect(303, CURL_REDIR_POST_303), PHP_EOL;

echo "ALL on, code 301: ";
echo do_redirect(301, CURL_REDIR_POST_ALL), PHP_EOL;

echo "ALL on, code 302: ";
echo do_redirect(302, CURL_REDIR_POST_ALL), PHP_EOL;

echo "ALL on, code 303: ";
echo do_redirect(303, CURL_REDIR_POST_ALL), PHP_EOL;

?>
--EXPECT--
default 301: string(0) ""
default 302: string(0) ""
default 303: string(0) ""
301 on, code 301: string(7) "foo=bar"
301 on, code 302: string(0) ""
301 on, code 303: string(0) ""
302 on, code 301: string(0) ""
302 on, code 302: string(7) "foo=bar"
302 on, code 303: string(0) ""
303 on, code 301: string(0) ""
303 on, code 302: string(0) ""
303 on, code 303: string(7) "foo=bar"
ALL on, code 301: string(7) "foo=bar"
ALL on, code 302: string(7) "foo=bar"
ALL on, code 303: string(7) "foo=bar"
