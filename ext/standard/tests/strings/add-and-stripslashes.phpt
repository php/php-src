--TEST--
addslashes() and stripslashes() functions, normal and sybase-style
--FILE--
<?php

$input = '';
for($i=0; $i<512; $i++) {
	$input .= chr($i%256);
}

echo "Normal: ";
ini_set('magic_quotes_sybase', 0);
if($input === stripslashes(addslashes($input))) {
	echo "OK\n";
} else {
	echo "FAILED\n";
}

echo "Sybase: ";
ini_set('magic_quotes_sybase', 1);
if($input === stripslashes(addslashes($input))) {
	echo "OK\n";
} else {
	echo "FAILED\n";
}

?>
--EXPECT--
Normal: OK
Sybase: OK
