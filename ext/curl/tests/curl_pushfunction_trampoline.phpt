--TEST--
Test trampoline for curl option CURLMOPT_PUSHFUNCTION
--EXTENSIONS--
curl
--XLEAK--
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

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
	    return CURL_PUSH_OK;
    }
}
$o = new TrampolineTest();
$callback = [$o, 'trampoline'];

$mh = curl_multi_init();

curl_multi_setopt($mh, CURLMOPT_PIPELINING, CURLPIPE_MULTIPLEX);
curl_multi_setopt($mh, CURLMOPT_PUSHFUNCTION, $callback);

$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "https://localhost/serverpush");
curl_setopt($ch, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

curl_multi_add_handle($mh, $ch);

$responses = [];
$active = null;
do {
    $status = curl_multi_exec($mh, $active);

    do {
        $info = curl_multi_info_read($mh);
        if (false !== $info && $info['msg'] == CURLMSG_DONE) {
            $handle = $info['handle'];
            if ($handle !== null) {
		        $responses[] = curl_multi_getcontent($info['handle']);
                curl_multi_remove_handle($mh, $handle);
                curl_close($handle);
            }
        }
    } while ($info);
} while (count($responses) !== 2);

curl_multi_close($mh);

sort($responses);
print_r($responses);
?>
--EXPECT--
Trampoline for trampoline
Array
(
    [0] => main response
    [1] => pushed response
)
