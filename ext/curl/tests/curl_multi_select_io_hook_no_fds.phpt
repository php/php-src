--TEST--
curl_multi_select() under IO hooks returns at once without descriptors, as curl_multi_wait() does
--EXTENSIONS--
curl
--FILE--
<?php
include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';

final class Tracing extends Scheduler
{
    public int $ops = 0;
    public function run(\Io\Operation $op): \Io\Completion
    {
        $this->ops++;
        return parent::run($op);
    }
}

$scheduler = new Tracing();
Io\Hooks\set_hooks($scheduler);
$scheduler->spawn(function () {
    $mh = curl_multi_init();
    var_dump(curl_multi_select($mh, 10.0));
});
$scheduler->loop();
Io\Hooks\set_hooks(null);
var_dump($scheduler->ops);
?>
--EXPECT--
int(0)
int(0)
