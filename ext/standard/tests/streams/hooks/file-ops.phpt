--TEST--
IO hooks: regular file reads, writes and fsync reach a provider with the Files capability only
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

final class Tracing extends Scheduler
{
    public array $seen = [];
    public array $problems = [];

    public function run(\Io\Operation $op): \Io\Completion
    {
        $this->seen[$op::class] = true;
        if ($op instanceof \Io\Operation\Read || $op instanceof \Io\Operation\Write) {
            /* -1 is the current position; a Windows overlapped file keeps none, so the
             * wrapper passes the offset it tracks */
            $offsetOk = PHP_OS_FAMILY === 'Windows' ? $op->getOffset() >= 0 : $op->getOffset() === -1;
            if ($op->getLength() <= 0 || !$offsetOk) {
                $this->problems[] = $op::class;
            }
        } elseif ($op instanceof \Io\Operation\Fsync) {
            if ($op->getHandle() === null || $op->getTimeout() !== null) {
                $this->problems[] = $op::class;
            }
            $this->seen['dataonly=' . var_export($op->isDataOnly(), true)] = true;
        }
        return parent::run($op);
    }
}

$scheduler = new Tracing(null, [Io\Hooks\Capability::Files]);
Io\Hooks\set_hooks($scheduler);
$files = in_array(Io\Hooks\Capability::Files, $scheduler->getCapabilities(), true);

$file = tempnam(sys_get_temp_dir(), 'iohooks');
$scheduler->spawn(function () use ($file) {
    $data = str_repeat("line of text\n", 20000);
    $f = fopen($file, 'w');
    fwrite($f, $data);
    fsync($f);
    fdatasync($f);
    fclose($f);
    $f = fopen($file, 'r');
    $read = '';
    while (!feof($f)) {
        $read .= fread($f, 65536);
    }
    fclose($f);
    var_dump($read === $data);
});
$scheduler->loop();
unlink($file);

// Without Files the file operations stay synchronous and never reach the provider
$seen = array_keys($scheduler->seen);
sort($seen);
$expected = $files
    ? [Io\Operation\Fsync::class, Io\Operation\Read::class, Io\Operation\Write::class, 'dataonly=false', 'dataonly=true']
    : [];
sort($expected);
var_dump($seen === $expected);
var_dump($scheduler->problems);
?>
--EXPECT--
bool(true)
bool(true)
array(0) {
}
