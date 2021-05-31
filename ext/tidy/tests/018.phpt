--TEST--
binary safety
--EXTENSIONS--
tidy
--FILE--
<?php
$x = tidy_repair_string("<p>abra\0cadabra</p>",
    array(	'show-body-only' => true,
            'clean' => false,
            'newline' => "\n")
    );
var_dump($x);
?>
--EXPECT--
string(19) "<p>abracadabra</p>
"
