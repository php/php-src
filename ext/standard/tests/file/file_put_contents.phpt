--TEST--
file_put_contents() and invalid parameters
--FILE--
<?php
class foo {
    function __toString() {
        return __METHOD__;
    }
}
$file = dirname(__FILE__)."/file_put_contents.txt";

$context = stream_context_create();

var_dump(file_put_contents($file, $context));
var_dump(file_put_contents($file, new stdClass));
var_dump(file_put_contents($file, new foo));
$fp = fopen($file, "r");
var_dump(file_put_contents($file, "string", 0, $fp));

@unlink($file);

echo "Done\n";
?>
--EXPECTF--	
Warning: file_put_contents(): supplied resource is not a valid stream resource in %s on line %d
bool(false)
bool(false)
int(15)

Warning: file_put_contents(): supplied resource is not a valid Stream-Context resource in %s on line %d
int(6)
Done
