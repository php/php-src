--TEST--
Bug #77193 Infinite loop in preg_replace_callback
--SKIPIF--
<?php
	if (!extension_loaded("filter")) {
		die("skip need filter extension");
	}
?>
--FILE--
<?php
$text = '{CCM:CID_2}';
echo '1';
$mt = array();
preg_replace_callback(
	'/([0-9]+)/i',
	function ($matches) {
		echo $matches[1];
		filter_var('http', FILTER_VALIDATE_REGEXP, ['options' => ['regexp' => '/^http$/i']]);
	},
	$text
);
echo '3', "\n";
?>
===DONE===
--EXPECT--
123
===DONE===
