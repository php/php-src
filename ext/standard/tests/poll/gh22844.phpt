--TEST--
GH-22844 (StreamPollHandle accessors are safe after the stream is closed)
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

[$r, $w] = pt_new_socket_pair();
$context = pt_new_stream_poll();
$handle = new StreamPollHandle($r);

echo "before close:\n";
echo "isValid: "; var_dump($handle->isValid());
echo "getStream is resource: "; var_dump(is_resource($handle->getStream()));

fclose($r);
fclose($w);

echo "after close:\n";
echo "isValid: "; var_dump($handle->isValid());
echo "getStream: "; var_dump($handle->getStream());

try {
    $context->add($handle, [Io\Poll\Event::Read]);
    echo "add: no exception\n";
} catch (\Throwable $e) {
    echo "add: ", $e::class, ": ", $e->getMessage(), "\n";
}

echo "done\n";
?>
--EXPECT--
before close:
isValid: bool(true)
getStream is resource: bool(true)
after close:
isValid: bool(false)
getStream: NULL
add: Io\Poll\InvalidHandleException: Invalid handle for polling
done
