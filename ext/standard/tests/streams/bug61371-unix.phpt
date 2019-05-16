--TEST--
Bug #61371: stream_context_create() causes memory leaks on use streams_socket_create
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == 'WIN' ) {
	die('skip non windows test');
}
--FILE--
<?php
function test($doFclose) {
$previous = null;
$current = null;
for($test=1;$test<=3;$test++) {
	$current = memory_get_usage(true);
	if (!is_null($previous)) {
		var_dump($previous == $current);
	}
	$previous = $current;
	echo 'memory: '.round($current / 1024, 0)."kb\n";
	for($i=0;$i<=100;$i++) {
		$context = stream_context_create(array());
		$stream = stream_socket_client('udp://0.0.0.0:80', $errno, $errstr, 10, STREAM_CLIENT_CONNECT, $context);
		if ($doFclose) fclose($stream);
		unset($context);
		unset($stream);
		unset($errno);
		unset($errstr);
	}
}
}

test(true);
test(false);
?>
--EXPECTF--
memory: %dkb
bool(true)
memory: %dkb
bool(true)
memory: %dkb
memory: %dkb
bool(true)
memory: %dkb
bool(true)
memory: %dkb
