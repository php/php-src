--TEST--
json_encode() with JSON_PRETTY_PRINT
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php
function encode_decode($json) {
	$struct = json_decode($json);
	$pretty = json_encode($struct, JSON_PRETTY_PRINT);
	echo "$pretty\n";
	$pretty = json_decode($pretty);
	printf("Match: %d\n", $pretty == $struct);
}

encode_decode('[1,2,3,[1,2,3]]');
encode_decode('{"a":1,"b":[1,2],"c":{"d":42}}');
?>
--EXPECT--
[
    1,
    2,
    3,
    [
        1,
        2,
        3
    ]
]
Match: 1
{
    "a": 1,
    "b": [
        1,
        2
    ],
    "c": {
        "d": 42
    }
}
Match: 1
