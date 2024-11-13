--TEST--
Test trampoline for curl option CURLOPT_FNMATCH_FUNCTION
--EXTENSIONS--
curl
--SKIPIF--
<?php
exit("skip: cannot properly test CURLOPT_FNMATCH_FUNCTION");
?>
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
	    return CURL_FNMATCHFUNC_NOMATCH;
    }
}
$o = new TrampolineTest();
$callback = [$o, 'trampoline'];

$url = "ftp://{$host}/file*";
//$url = "ftp://ftp.example.com/file*";
$ch = curl_init($url);
curl_setopt($ch, CURLOPT_WILDCARDMATCH, 1);
curl_setopt($ch, CURLOPT_FNMATCH_FUNCTION, $callback);
echo curl_exec($ch), PHP_EOL;

?>
--EXPECT--
Trampoline for trampoline
Hello World!
Hello World!
