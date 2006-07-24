<?php

$zip = zip_open('examples/test1.zip');
var_dump($zip);

if ($zip) {
	$i = 0;
	while ($zip_entry = zip_read($zip)) {
		var_dump($zip_entry);
		$txt = zip_entry_read($zip_entry, 10);
        echo $i . ": " . $txt . "size: " . zip_entry_filesize($zip_entry) . 
			"comp_method: " . zip_entry_compressionmethod($zip_entry) . 
			"\n";
		$i++;
	}
	var_dump($zip_entry);
}
