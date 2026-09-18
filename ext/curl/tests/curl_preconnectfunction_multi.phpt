--TEST--
CURLOPT_PRECONNECTFUNCTION under curl_multi
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';

$host = curl_cli_server_start();

$mh = curl_multi_init();
$handles = [];
$calls = [];

foreach ([true, false] as $i => $allow) {
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, "{$host}/get.inc?test=method");
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_FORBID_REUSE, true);
    $calls[$i] = 0;
    curl_setopt($ch, CURLOPT_PRECONNECTFUNCTION, function () use ($allow, $i, &$calls): bool {
        $calls[$i]++;
        return $allow;
    });
    curl_multi_add_handle($mh, $ch);
    $handles[$i] = $ch;
}

do {
    curl_multi_exec($mh, $active);
    if ($active) {
        curl_multi_select($mh);
    }
} while ($active);

$results = [];
while ($info = curl_multi_info_read($mh)) {
    $results[array_search($info['handle'], $handles, true)] = $info['result'];
}
ksort($results);

var_dump($calls[0] >= 1, $calls[1] >= 1);
var_dump(curl_multi_getcontent($handles[0]));
var_dump($results[0] === CURLE_OK);
var_dump($results[1] === CURLE_COULDNT_CONNECT);

foreach ($handles as $ch) {
    curl_multi_remove_handle($mh, $ch);
}

echo "Done";
?>
--EXPECT--
bool(true)
bool(true)
string(3) "GET"
bool(true)
bool(true)
Done
