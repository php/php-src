--TEST--
file_put_contents() and invalid parameters
--FILE--
<?php
class foo {
    function __toString() {
        return __METHOD__;
    }
}
$file = __DIR__."/file_put_contents.txt";

$context = stream_context_create();

try {
    var_dump(file_put_contents($file, $context));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump(file_put_contents($file, new stdClass));
var_dump(file_put_contents($file, new foo));
$fp = fopen($file, "r");
try {
    var_dump(file_put_contents($file, "string", 0, $fp));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "Done\n";
?>
--CLEAN--
<?php
$file = __DIR__."/file_put_contents.txt";
unlink($file);
?>
--EXPECT--
TypeError: file_put_contents(): supplied resource is not a valid stream resource
bool(false)
int(15)
TypeError: file_put_contents(): supplied resource is not a valid Stream-Context resource
Done
