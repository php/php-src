<?php

const WORKER_ARGV_VALUE = 'RUN_WORKER';

const WORKER_DEFAULT_NAME = 'server';

function phpt_notify($worker = WORKER_DEFAULT_NAME)
{
    ServerClientTestCase::getInstance()->notify($worker);
}

function phpt_wait($worker = WORKER_DEFAULT_NAME, $timeout = null)
{
    ServerClientTestCase::getInstance()->wait($worker, $timeout);
}

function phpt_has_sslv3() {
    static $result = null;
    if (!is_null($result)) {
        return $result;
    }
    $server = @stream_socket_server('sslv3://127.0.0.1:10013');
    if ($result = !!$server) {
        fclose($server);
    }
    return $result;
}

function phpt_extract_tls_records($rawData) {
    $records = [];
    $offset = 0;
    $dataLength = strlen($rawData);

    while ($offset < $dataLength) {
        // Ensure there's enough data left for the header.
        if ($offset + 5 > $dataLength) {
            break;
        }

        // Extract the length of the current record.
        $length = unpack("n", substr($rawData, $offset + 3, 2))[1];

        // Check if the total length is within the bounds of the rawData.
        if ($offset + 5 + $length > $dataLength) {
            break;
        }

        // Extract the record and add it to the records array.
        $records[] = substr($rawData, $offset, 5 + $length);

        // Move the offset past the current record.
        $offset += 5 + $length;
    }

    return $records;
}

/**
 * This is a singleton to let the wait/notify functions work
 * I know it's horrible, but it's a means to an end
 */
class ServerClientTestCase
{
    private $isWorker = false;

    private $workerHandle = [];

    private $workerStdIn = [];

    private $workerStdOut = [];

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

    private function spawnWorkerProcess($worker, $code)
    {
        if (defined("PHP_WINDOWS_VERSION_MAJOR")) {
            $ini = php_ini_loaded_file();
            $cmd = sprintf(
                '%s %s "%s" %s',
                PHP_BINARY, $ini ? "-n -c $ini" : "",
                __FILE__,
                WORKER_ARGV_VALUE
            );
        } else {
            $cmd = sprintf(
                '%s %s "%s" %s %s',
                PHP_BINARY,
                getenv('TEST_PHP_EXTRA_ARGS'),
                __FILE__,
                WORKER_ARGV_VALUE,
                $worker
            );
        }
        $this->workerHandle[$worker] = proc_open(
            $cmd,
            [['pipe', 'r'], ['pipe', 'w'], STDERR],
            $pipes
        );
        $this->workerStdIn[$worker] = $pipes[0];
        $this->workerStdOut[$worker] = $pipes[1];

        fwrite($this->workerStdIn[$worker], $code . "\n---\n");
    }

    private function cleanupWorkerProcess($worker)
    {
        fclose($this->workerStdIn[$worker]);
        fclose($this->workerStdOut[$worker]);
        proc_close($this->workerHandle[$worker]);
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

    public function run($masterCode, $workerCode)
    {
        if (!is_array($workerCode)) {
            $workerCode = [WORKER_DEFAULT_NAME => $workerCode];
        }
        foreach ($workerCode as $worker => $code) {
            $this->spawnWorkerProcess($worker, $this->stripPhpTagsFromCode($code));
        }
        eval($this->stripPhpTagsFromCode($masterCode));
        foreach ($workerCode as $worker => $code) {
            $this->cleanupWorkerProcess($worker);
        }
    }

    public function wait($worker, $timeout = null)
    {
        $handle = $this->isWorker ? STDIN : $this->workerStdOut[$worker];
        if ($timeout === null) {
            fgets($handle);
            return true;
        }

        stream_set_blocking($handle, false);
        $read = [$handle];
        $result = stream_select($read, $write, $except, $timeout);
        if (!$result) {
            return false;
        }

        fgets($handle);
        stream_set_blocking($handle, true);
        return true;
    }

    public function notify($worker)
    {
        fwrite($this->isWorker ? STDOUT : $this->workerStdIn[$worker], "\n");
    }
}

if (isset($argv[1]) && $argv[1] === WORKER_ARGV_VALUE) {
    ServerClientTestCase::getInstance(true)->runWorker();
}
