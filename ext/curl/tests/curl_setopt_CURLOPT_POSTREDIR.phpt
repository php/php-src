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
    curl_setopt($ch, CURLOPT_POSTFIELDS, 'postdata was kept in redirect');
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
    if ($postredir_value !== null) {
        curl_setopt($ch, CURLOPT_POSTREDIR, $postredir_value);
    }
    return trim(curl_exec($ch));
}

$options = [null, CURL_REDIR_POST_301, CURL_REDIR_POST_302, CURL_REDIR_POST_303, CURL_REDIR_POST_ALL, 0];
$codes = [301, 302, 303];

foreach ($options as $option) {
    foreach ($codes as $code) {
        echo "code: $code; option ", var_export($option, true), ': ';
        echo do_redirect($code, $option), "\n";
    }
}

?>
--EXPECT--
code: 301; option NULL: string(0) ""
code: 302; option NULL: string(0) ""
code: 303; option NULL: string(0) ""
code: 301; option 1: string(29) "postdata was kept in redirect"
code: 302; option 1: string(0) ""
code: 303; option 1: string(0) ""
code: 301; option 2: string(0) ""
code: 302; option 2: string(29) "postdata was kept in redirect"
code: 303; option 2: string(0) ""
code: 301; option 4: string(0) ""
code: 302; option 4: string(0) ""
code: 303; option 4: string(29) "postdata was kept in redirect"
code: 301; option 7: string(29) "postdata was kept in redirect"
code: 302; option 7: string(29) "postdata was kept in redirect"
code: 303; option 7: string(29) "postdata was kept in redirect"
code: 301; option 0: string(0) ""
code: 302; option 0: string(0) ""
code: 303; option 0: string(0) ""
