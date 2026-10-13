--TEST--
Io\Poll: StreamPollWeakHandle::getStream() returns null after stream is freed
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

[$r, $w] = pt_new_socket_pair();
$handle = StreamPollWeakHandle::create($r);

var_dump(is_resource($handle->getStream()));

fclose($r);
fclose($w);

var_dump($handle->getStream());
echo "done\n";
?>
--EXPECT--
bool(true)
NULL
done
