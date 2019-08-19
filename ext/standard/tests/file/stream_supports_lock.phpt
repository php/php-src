--TEST--
stream_supports_lock
--INI--
allow_url_fopen=1
--FILE--
<?php
$fp = fopen(__FILE__, "r");
var_dump($fp);
var_dump(stream_supports_lock($fp));
fclose($fp);

$fp = fopen("file://" . __FILE__, "r");
var_dump($fp);
var_dump(stream_supports_lock($fp));
fclose($fp);

$fp = fopen("php://memory", "r");
var_dump($fp);
var_dump(stream_supports_lock($fp));
fclose($fp);

$fp = fopen('data://text/plain,foobar', 'r');
var_dump($fp);
var_dump(stream_supports_lock($fp));
fclose($fp);

$sock = stream_context_create();
var_dump($sock);
try {
    var_dump(stream_supports_lock($sock));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECTF--
resource(%d) of type (stream)
bool(true)
resource(%d) of type (stream)
bool(true)
resource(%d) of type (stream)
bool(false)
resource(%d) of type (stream)
bool(false)
resource(%d) of type (stream-context)
stream_supports_lock(): supplied resource is not a valid stream resource
Done
