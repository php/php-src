--TEST--
CURLOPT_OPENSOCKETFUNCTION: freeing the handle while its connection stays pooled
--EXTENSIONS--
curl
--FILE--
<?php
include 'keepalive_server.inc';

$host = curl_keepalive_server_start();

/* The callback holds a reference to the handle it is set on: releasing the easy
 * handle while its connection stays cached in a share or a multi handle must stay
 * clean. The server keeps connections open and answers with the number it has
 * accepted, so the responses show that the connection really was pooled and
 * reused. The point of this test is to exercise the teardown order under
 * ASAN/valgrind in CI. */
$sh = curl_share_init();
curl_share_setopt($sh, CURLSHOPT_SHARE, CURL_LOCK_DATA_CONNECT);

for ($i = 0; $i < 3; $i++) {
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, "{$host}/");
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_PROXY, '');
    curl_setopt($ch, CURLOPT_SHARE, $sh);
    curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, function () use ($ch): bool {
        return true;
    });
    var_dump(curl_exec($ch));
    unset($ch);
}

unset($sh);

$mh = curl_multi_init();
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "{$host}/");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_PROXY, '');
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, function () use ($ch): bool {
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
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "2"
Done
