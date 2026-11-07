--TEST--
CURLMOPT_PUSHFUNCTION deny must not leak the pushed handle
--EXTENSIONS--
curl
--SKIPIF--
<?php
include 'skipif-nocaddy.inc';

$curl_version = curl_version();
if ($curl_version['version_number'] < 0x080100) {
    exit("skip: test may crash with curl < 8.1.0");
}
?>
--FILE--
<?php
$push_count = 0;

function do_request(): void {
    $callback = function ($parent_ch, $pushed_ch, array $headers) {
        $GLOBALS['push_count']++;
        return CURL_PUSH_DENY;
    };

    $mh = curl_multi_init();
    curl_multi_setopt($mh, CURLMOPT_PIPELINING, CURLPIPE_MULTIPLEX);
    curl_multi_setopt($mh, CURLMOPT_PUSHFUNCTION, $callback);

    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, "https://localhost/serverpush");
    curl_setopt($ch, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_multi_add_handle($mh, $ch);

    $done = false;
    do {
        curl_multi_exec($mh, $active);

        do {
            $info = curl_multi_info_read($mh);
            if (false !== $info && $info['msg'] == CURLMSG_DONE) {
                $handle = $info['handle'];
                if ($handle !== null) {
                    curl_multi_remove_handle($mh, $handle);
                    curl_close($handle);
                    $done = true;
                }
            }
        } while ($info);
    } while (!$done);

    curl_multi_close($mh);
}

$iterations = 200;
$bytes_per_iteration_limit = 512;

do_request();
gc_collect_cycles();

$before = memory_get_usage();
for ($i = 0; $i < $iterations; $i++) {
    do_request();
}
gc_collect_cycles();
$growth = memory_get_usage() - $before;

if ($push_count < $iterations) {
    printf("too few pushes handled: %d\n", $push_count);
} else {
    echo "all pushes denied\n";
}
if ($growth > $iterations * $bytes_per_iteration_limit) {
    printf("leaked %d bytes (%d bytes/iteration)\n", $growth, (int) ($growth / $iterations));
} else {
    echo "no leak detected\n";
}
?>
--EXPECT--
all pushes denied
no leak detected
