--TEST--
array curl_multi_get_handles ( CurlMultiHandle $mh );
--EXTENSIONS--
curl
--FILE--
<?php
$urls = array(
    "file://".__DIR__."/curl_testdata1.txt",
    "file://".__DIR__."/curl_testdata2.txt",
);

$mh = curl_multi_init();
$map = new WeakMap();

foreach ($urls as $url) {
    echo "Initializing {$url}.", PHP_EOL;
    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_multi_add_handle($mh, $ch);
    printf("%d handles are attached\n", count(curl_multi_get_handles($mh)));
    $map[$ch] = $url;
}

do {
    $status = curl_multi_exec($mh, $active);
    if ($status !== CURLM_OK) {
        throw new \Exception(curl_multi_strerror(curl_multi_errno($mh)));
    }

    if ($active) {
        $activity = curl_multi_select($mh);
        if ($activity === -1) {
            throw new \Exception(curl_multi_strerror(curl_multi_errno($mh)));
        }
    }

    while (($info = curl_multi_info_read($mh)) !== false) {
        if ($info['msg'] === CURLMSG_DONE) {
            $handle = $info['handle'];
            $url = $map[$handle];
            echo "Request to {$url} finished.", PHP_EOL;
            printf("%d handles are attached\n", count(curl_multi_get_handles($mh)));
            curl_multi_remove_handle($mh, $handle);
            printf("%d handles are attached\n", count(curl_multi_get_handles($mh)));

            if ($info['result'] === CURLE_OK) {
                echo "Success.", PHP_EOL;
            } else {
                echo "Failure.", PHP_EOL;
            }
        }
    }
} while ($active);

printf("%d handles are attached\n", count(curl_multi_get_handles($mh)));

?>
--EXPECTF--
Initializing %scurl_testdata1.txt.
1 handles are attached
Initializing %scurl_testdata2.txt.
2 handles are attached
Request to %scurl_testdata%d.txt finished.
2 handles are attached
1 handles are attached
Success.
Request to %scurl_testdata%d.txt finished.
1 handles are attached
0 handles are attached
Success.
0 handles are attached
