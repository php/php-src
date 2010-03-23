<?php
$tag = 'en_US';
$r = enchant_broker_init();
$bprovides = enchant_broker_describe($r);
echo "Current broker provides the following backend(s):\n";
print_r($bprovides);


if (enchant_broker_dict_exists($r,$tag)) {
	$d = enchant_broker_request_dict($r, $tag);
	$dprovides = enchant_dict_describe($d);
	echo "dictionary $tag provides:\n";
	$spellerrors = enchant_dict_check($d, "soong");
	print_r($dprovides);
	echo "found $spellerrors spell errors\n";
	if (spellerrors) {
		$suggs = enchant_dict_suggest($d, "soong");
		echo "Suggestions for 'soong':";
		print_r($suggs);
	}
	enchant_broker_free_dict($d);
} else {
}
enchant_broker_free($r);
?>
