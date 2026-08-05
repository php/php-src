--TEST--
error when CURLOPT_READFUNCTION returns an integer
--EXTENSIONS--
curl
--FILE--
<?php
function custom_readfunction($oCurl, $hReadHandle, $iMaxOut)
{
    static $size = 2;
    return $size--;
}

include 'server.inc';
$host = curl_cli_server_start();
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "{$host}/get.php?test=post");
curl_setopt($ch, CURLOPT_POST, ['f' => 'f']);
curl_setopt($ch, CURLOPT_TIMEOUT, 2);
curl_setopt($ch, CURLOPT_READFUNCTION, "custom_readfunction" );

try {
    curl_exec($ch);
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}
var_dump(curl_error($ch));
?>
--EXPECT--
The CURLOPT_READFUNCTION callback must return a string or CURL_READFUNC_ABORT or CURL_READFUNC_PAUSE
string(29) "operation aborted by callback"
