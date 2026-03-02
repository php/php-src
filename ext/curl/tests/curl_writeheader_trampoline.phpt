--TEST--
Test trampoline for curl option CURLOPT_HEADERFUNCTION
--EXTENSIONS--
curl
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

include 'server.inc';
$host = curl_cli_server_start();

$ch = curl_init();
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch, CURLOPT_HEADERFUNCTION, $callback);
curl_setopt($ch, CURLOPT_URL, $host);
curl_exec($ch);

?>
--EXPECT--
Trampoline for trampoline
