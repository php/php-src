--TEST--
addslashes() and stripslashes() functions
--FILE--
<?php

$input = '';
for($i=0; $i<512; $i++) {
    $input .= chr($i%256);
}

echo "Normal: ";
if($input === stripslashes(addslashes($input))) {
    echo "OK\n";
} else {
    echo "FAILED\n";
}

?>
--EXPECT--
Normal: OK
