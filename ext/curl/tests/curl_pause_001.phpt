--TEST--
Test CURL_READFUNC_PAUSE and curl_pause()
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) {
    // finished in 1.32s in a best-of-3 on an idle Intel Xeon X5670 on PHP 8.3.0-dev
    die("skip slow test");
}
?>
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();

class Input {
	private static $RESPONSES = [
		'Foo bar ',
		CURL_READFUNC_PAUSE,
		'baz qux',
		null
	];
	private int $res = 0;
	public function __invoke($ch, $hReadHandle, $iMaxOut)
	{
		return self::$RESPONSES[$this->res++];
	}
}

$inputHandle = fopen(__FILE__, 'r');

$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "{$host}/get.inc?test=input");
curl_setopt($ch, CURLOPT_UPLOAD,       1);
curl_setopt($ch, CURLOPT_READFUNCTION, new Input);
curl_setopt($ch, CURLOPT_INFILE,       $inputHandle);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

$mh = curl_multi_init();
curl_multi_add_handle($mh, $ch);
do {
	$status = curl_multi_exec($mh, $active);
	curl_pause($ch, CURLPAUSE_CONT);
	if ($active) {
		usleep(100);
		curl_multi_select($mh);
	}
} while ($active && $status == CURLM_OK);

echo curl_multi_getcontent($ch);
?>
--EXPECT--
string(15) "Foo bar baz qux"
