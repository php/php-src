--TEST--
CURLOPT_PRECONNECTFUNCTION: freeing the handle while its connection stays pooled
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';

$host = curl_cli_server_start();

/* The callback holds a reference to the handle it is set on: releasing the easy
 * handle while the connection is still cached in a share must stay clean. The
 * assertions below only prove the transfers still work; the point of this test is
 * to exercise the teardown order under ASAN/valgrind in CI. */
$sh = curl_share_init();
curl_share_setopt($sh, CURLSHOPT_SHARE, CURL_LOCK_DATA_CONNECT);

for ($i = 0; $i < 3; $i++) {
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, "{$host}/get.inc?test=method");
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_SHARE, $sh);
    curl_setopt($ch, CURLOPT_PRECONNECTFUNCTION, function () use ($ch): bool {
        return true;
    });
    var_dump(curl_exec($ch));
    unset($ch);
}

unset($sh);

$mh = curl_multi_init();
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "{$host}/get.inc?test=method");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_PRECONNECTFUNCTION, function () use ($ch): bool {
    return true;
});
curl_multi_add_handle($mh, $ch);

do {
    curl_multi_exec($mh, $active);
    if ($active) {
        curl_multi_select($mh);
    }
} while ($active);

var_dump(curl_multi_getcontent($ch));
curl_multi_remove_handle($mh, $ch);
unset($ch, $mh);

echo "Done";
?>
--EXPECT--
string(3) "GET"
string(3) "GET"
string(3) "GET"
string(3) "GET"
Done
