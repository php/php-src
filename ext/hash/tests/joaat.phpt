--TEST--
Hash: Jenkins's one-at-a-time
--FILE--
<?php
$tests = array(
	array("hello world", "3e4a5a57"),
	array("", 0),
	array("", "000000"),
	array("a", "ca2e9442"),
	array("aa", "7081738e"),
);

$i = 0;
$pass = true;

foreach ($tests as $test) {
	++$i;

	$result = hash("joaat", $test[0]);
	if ($result != $test[1]) {
		echo "Iteration " . $i . " failed - expected '" . $test[1] . "', got '" . $result . "' for '" . $test[1] . "'\n";

		$pass = false;
	}
}

if($pass) {
    echo "PASS";
}
?>
--EXPECT--
PASS
