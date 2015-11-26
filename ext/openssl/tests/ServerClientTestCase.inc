<?php

const WORKER_ARGV_VALUE = 'RUN_WORKER';

function phpt_notify()
{
    ServerClientTestCase::getInstance()->notify();
}

function phpt_wait()
{
    ServerClientTestCase::getInstance()->wait();
}

/**
 * This is a singleton to let the wait/notify functions work
 * I know it's horrible, but it's a means to an end
 */
class ServerClientTestCase
{
    private $isWorker = false;

    private $workerHandle;

    private $workerStdIn;

    private $workerStdOut;

    private static $instance;

    public static function getInstance($isWorker = false)
    {
        if (!isset(self::$instance)) {
            self::$instance = new self($isWorker);
        }

        return self::$instance;
    }

    public function __construct($isWorker = false)
    {
        if (!isset(self::$instance)) {
            self::$instance = $this;
        }

        $this->isWorker = $isWorker;
    }

    private function spawnWorkerProcess($code)
    {
        $cmd = sprintf('%s "%s" %s', PHP_BINARY, __FILE__, WORKER_ARGV_VALUE);

        $this->workerHandle = proc_open($cmd, [['pipe', 'r'], ['pipe', 'w'], STDERR], $pipes);
        $this->workerStdIn = $pipes[0];
        $this->workerStdOut = $pipes[1];

        fwrite($this->workerStdIn, $code . "\n---\n");
    }

    private function cleanupWorkerProcess()
    {
        fclose($this->workerStdIn);
        fclose($this->workerStdOut);
        proc_close($this->workerHandle);
    }

    private function stripPhpTagsFromCode($code)
    {
        return preg_replace('/^\s*<\?(?:php)?|\?>\s*$/i', '', $code);
    }

    public function runWorker()
    {
        $code = '';

        while (1) {
            $line = fgets(STDIN);

            if (trim($line) === "---") {
                break;
            }

            $code .= $line;
        }

        eval($code);
    }

    public function run($proc1Code, $proc2Code)
    {
        $this->spawnWorkerProcess($this->stripPhpTagsFromCode($proc2Code));
        eval($this->stripPhpTagsFromCode($proc1Code));
        $this->cleanupWorkerProcess();
    }
    
    public function wait()
    {
        fgets($this->isWorker ? STDIN : $this->workerStdOut);
    }

    public function notify()
    {
        fwrite($this->isWorker ? STDOUT : $this->workerStdIn, "\n");
    }
}

if (isset($argv[1]) && $argv[1] === WORKER_ARGV_VALUE) {
    ServerClientTestCase::getInstance(true)->runWorker();
}
