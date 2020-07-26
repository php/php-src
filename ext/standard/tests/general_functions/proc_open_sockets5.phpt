--TEST--
proc_open() with sockets
--FILE--
<?php

// 0: chunks size, 1: chunk count
$settings = [
	[1, 10],
	[1, 1000],
	[1000, 10],
	[1000, 1000],
	[5000, 10],
	[5000, 1000],
	[20000, 1],
	[20000, 10],
	[20000, 100]
];

echo "START\n";

foreach ($settings as list ($size, $count)) {
	$cmd = [
		getenv("TEST_PHP_EXECUTABLE"),
		__DIR__ . '/proc_open_sockets5.inc'
	];

	$spec = [
		['socket'],
		['socket']
	];

	$proc = proc_open($cmd, $spec, $pipes, null, null, [
		'threaded_pipes' => true
	]);

	$hash = hash_init('md5');
	$chunk = str_repeat('A', $size);

	for ($i = 0; $i < $count; $i++) {
		hash_update($hash, $chunk);
		fwrite($pipes[0], $chunk);
	}

	fclose($pipes[0]);

	$a = trim(stream_get_contents($pipes[1]));
	$b = hash_final($hash);
	
	if ($a !== $b) {
		throw new Error(sprintf('Test failed with chunk size %d and %d chunks', $size, $count));
	}
}

echo "DONE\n";

?>
--EXPECTF--
START
DONE
