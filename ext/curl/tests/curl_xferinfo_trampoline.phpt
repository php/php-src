--TEST--
Test trampoline for curl option CURLOPT_XFERINFOFUNCTION
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        static $done = false;
        if (!$done) {
            echo 'Trampoline for ', $name, PHP_EOL;
            $done = true;
        }
	    return CURL_PUSH_OK;
    }
}
$o = new TrampolineTest();
$callback = [$o, 'trampoline'];

$url = "{$host}/get.inc";
$ch = curl_init($url);
curl_setopt($ch, CURLOPT_NOPROGRESS, 0);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch, CURLOPT_XFERINFOFUNCTION, $callback);
echo curl_exec($ch), PHP_EOL;

?>
--EXPECT--
Trampoline for trampoline
Hello World!
Hello World!
