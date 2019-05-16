<?php

namespace FPM;

use Adoy\FastCGI\Client;

require_once 'fcgi.inc';
require_once 'logtool.inc';
require_once 'response.inc';

class Tester
{
    /**
     * Config directory for included files.
     */
    const CONF_DIR = __DIR__ . '/conf.d';

    /**
     * File extension for access log.
     */
    const FILE_EXT_LOG_ACC = 'acc.log';

    /**
     * File extension for error log.
     */
    const FILE_EXT_LOG_ERR = 'err.log';

    /**
     * File extension for slow log.
     */
    const FILE_EXT_LOG_SLOW = 'slow.log';

    /**
     * File extension for PID file.
     */
    const FILE_EXT_PID = 'pid';

    /**
     * @var array
     */
    static private $supportedFiles = [
        self::FILE_EXT_LOG_ACC,
        self::FILE_EXT_LOG_ERR,
        self::FILE_EXT_LOG_SLOW,
        self::FILE_EXT_PID,
        'src.php',
        'ini',
        'skip.ini',
        '*.sock',
    ];

    /**
     * @var array
     */
    static private $filesToClean = ['.user.ini'];

    /**
     * @var bool
     */
    private $debug;

    /**
     * @var array
     */
    private $clients;

    /**
     * @var LogTool
     */
    private $logTool;

    /**
     * Configuration template
     *
     * @var string
     */
    private $configTemplate;

    /**
     * The PHP code to execute
     *
     * @var string
     */
    private $code;

    /**
     * @var array
     */
    private $options;

    /**
     * @var string
     */
    private $fileName;

    /**
     * @var resource
     */
    private $masterProcess;

    /**
     * @var resource
     */
    private $outDesc;

    /**
     * @var array
     */
    private $ports = [];

    /**
     * @var string
     */
    private $error;

    /**
     * The last response for the request call
     *
     * @var Response
     */
    private $response;

    /**
     * Clean all the created files up
     *
     * @param int $backTraceIndex
     */
    static public function clean($backTraceIndex = 1)
    {
        $filePrefix = self::getCallerFileName($backTraceIndex);
        if (substr($filePrefix, -6) === 'clean.') {
            $filePrefix = substr($filePrefix, 0, -6);
        }

        $filesToClean = array_merge(
            array_map(
                function($fileExtension) use ($filePrefix) {
                    return $filePrefix . $fileExtension;
                },
                self::$supportedFiles
            ),
            array_map(
                function($fileExtension) {
                    return __DIR__ . '/' . $fileExtension;
                },
                self::$filesToClean
            )
        );
        // clean all the root files
        foreach ($filesToClean as $filePattern) {
            foreach (glob($filePattern) as $filePath) {
                unlink($filePath);
            }
        }
        // clean config files
        if (is_dir(self::CONF_DIR)) {
            foreach(glob(self::CONF_DIR . '/*.conf') as $name) {
                unlink($name);
            }
            rmdir(self::CONF_DIR);
        }
    }

    /**
     * @param int $backTraceIndex
     * @return string
     */
    static private function getCallerFileName($backTraceIndex = 1)
    {
        $backtrace = debug_backtrace();
        if (isset($backtrace[$backTraceIndex]['file'])) {
            $filePath = $backtrace[$backTraceIndex]['file'];
        } else {
            $filePath = __FILE__;
        }

        return substr($filePath, 0, -strlen(pathinfo($filePath, PATHINFO_EXTENSION)));
    }

    /**
     * @return bool|string
     */
    static public function findExecutable()
    {
        $phpPath = getenv("TEST_PHP_EXECUTABLE");
        for ($i = 0; $i < 2; $i++) {
            $slashPosition = strrpos($phpPath, "/");
            if ($slashPosition) {
                $phpPath = substr($phpPath, 0, $slashPosition);
            } else {
                break;
            }
        }

        if ($phpPath && is_dir($phpPath)) {
            if (file_exists($phpPath."/fpm/php-fpm") && is_executable($phpPath."/fpm/php-fpm")) {
                /* gotcha */
                return $phpPath."/fpm/php-fpm";
            }
            $phpSbinFpmi = $phpPath."/sbin/php-fpm";
            if (file_exists($phpSbinFpmi) && is_executable($phpSbinFpmi)) {
                return $phpSbinFpmi;
            }
        }

        // try local php-fpm
        $fpmPath = dirname(__DIR__) . '/php-fpm';
        if (file_exists($fpmPath) && is_executable($fpmPath)) {
            return $fpmPath;
        }

        return false;
    }

    /**
     * Skip test if any of the supplied files does not exist.
     *
     * @param mixed $files
     */
    static public function skipIfAnyFileDoesNotExist($files)
    {
        if (!is_array($files)) {
            $files = array($files);
        }
        foreach ($files as $file) {
            if (!file_exists($file)) {
                die("skip File $file does not exist");
            }
        }
    }

    /**
     * Skip test if config file is invalid.
     *
     * @param string $configTemplate
     * @throws \Exception
     */
    static public function skipIfConfigFails(string $configTemplate)
    {
        $tester = new self($configTemplate, '', [], self::getCallerFileName());
        $testResult = $tester->testConfig();
        if ($testResult !== null) {
            self::clean(2);
            die("skip $testResult");
        }
    }

    /**
     * Skip test if IPv6 is not supported.
     */
    static public function skipIfIPv6IsNotSupported()
    {
        @stream_socket_client('tcp://[::1]:0', $errno);
        if ($errno != 111) {
            die('skip IPv6 is not supported.');
        }
    }

    /**
     * Skip if running on Travis.
     *
     * @param $message
     */
    static public function skipIfTravis($message)
    {
        if (getenv("TRAVIS")) {
            die('skip Travis: ' . $message);
        }
    }

    /**
     * Tester constructor.
     *
     * @param string|array $configTemplate
     * @param string $code
     * @param array $options
     * @param string $fileName
     */
    public function __construct(
        $configTemplate,
        string $code = '',
        array $options = [],
        $fileName = null
    ) {
        $this->configTemplate = $configTemplate;
        $this->code = $code;
        $this->options = $options;
        $this->fileName = $fileName ?: self::getCallerFileName();
        $this->logTool = new LogTool();
        $this->debug = (bool) getenv('TEST_FPM_DEBUG');
    }

    /**
     * @param string $ini
     */
    public function setUserIni(string $ini)
    {
        $iniFile = __DIR__ . '/.user.ini';
        file_put_contents($iniFile, $ini);
    }

    /**
     * Test configuration file.
     *
     * @return null|string
     * @throws \Exception
     */
    public function testConfig()
    {
        $configFile = $this->createConfig();
        $cmd = self::findExecutable() . ' -t -y ' . $configFile . ' 2>&1';
        exec($cmd, $output, $code);
        if ($code) {
            return preg_replace("/\[.+?\]/", "", $output[0]);
        }

        return null;
    }

    /**
     * Start PHP-FPM master process
     *
     * @param string $extraArgs
     * @return bool
     * @throws \Exception
     */
    public function start(string $extraArgs = '')
    {
        $configFile = $this->createConfig();
        $desc = $this->outDesc ? [] : [1 => array('pipe', 'w')];
        $asRoot = getenv('TEST_FPM_RUN_AS_ROOT') ? '--allow-to-run-as-root' : '';
        $cmd = self::findExecutable() . " $asRoot -F -O -y $configFile $extraArgs";
        /* Since it's not possible to spawn a process under linux without using a
         * shell in php (why?!?) we need a little shell trickery, so that we can
         * actually kill php-fpm */
        $this->masterProcess = proc_open(
            "killit () { kill \$child 2> /dev/null; }; " .
                "trap killit TERM; $cmd 2>&1 & child=\$!; wait",
            $desc,
            $pipes
        );
        register_shutdown_function(
            function($masterProcess) use($configFile) {
                @unlink($configFile);
                if (is_resource($masterProcess)) {
                    @proc_terminate($masterProcess);
                    while (proc_get_status($masterProcess)['running']) {
                        usleep(10000);
                    }
                }
            },
            $this->masterProcess
        );
        if (!$this->outDesc !== false) {
            $this->outDesc = $pipes[1];
        }

        return true;
    }

    /**
     * Run until needle is found in the log.
     *
     * @param string $needle
     * @param int $max
     * @return bool
     * @throws \Exception
     */
    public function runTill(string $needle, $max = 10)
    {
        $this->start();
        $found = false;
        for ($i = 0; $i < $max; $i++) {
            $line = $this->getLogLine();
            if (is_null($line)) {
                break;
            }
            if (preg_match($needle, $line) === 1) {
                $found = true;
                break;
            }
        }
        $this->close(true);

        if (!$found) {
            return $this->error("The search pattern not found");
        }

        return true;
    }

    /**
     * Check if connection works.
     *
     * @param string $host
     * @param null|string $successMessage
     * @param null|string $errorMessage
     * @param int $attempts
     * @param int $delay
     */
    public function checkConnection(
        $host = '127.0.0.1',
        $successMessage = null,
        $errorMessage = 'Connection failed',
        $attempts = 20,
        $delay = 50000
    ) {
        $i = 0;
        do {
            if ($i > 0 && $delay > 0) {
                usleep($delay);
            }
            $fp = @fsockopen($host, $this->getPort());
        } while ((++$i < $attempts) && !$fp);

        if ($fp) {
            $this->message($successMessage);
            fclose($fp);
        } else {
            $this->message($errorMessage);
        }
    }


    /**
     * Execute request with parameters ordered for better checking.
     *
     * @param string $address
     * @param string|null $successMessage
     * @param string|null $errorMessage
     * @param string $uri
     * @param string $query
     * @param array $headers
     * @return Response
     */
    public function checkRequest(
        string $address,
        string $successMessage = null,
        string $errorMessage = null,
        $uri = '/ping',
        $query = '',
        $headers = []
    ) {
        return $this->request($query, $headers, $uri, $address, $successMessage, $errorMessage);
    }

    /**
     * Execute and check ping request.
     *
     * @param string $address
     * @param string $pingPath
     * @param string $pingResponse
     */
    public function ping(
        string $address = '{{ADDR}}',
        string $pingResponse = 'pong',
        string $pingPath = '/ping'
    ) {
        $response = $this->request('', [], $pingPath, $address);
        $response->expectBody($pingResponse, 'text/plain');
    }

    /**
     * Execute and check status request(s).
     *
     * @param array $expectedFields
     * @param string|null $address
     * @param string $statusPath
     * @param mixed $formats
     * @throws \Exception
     */
    public function status(
        array $expectedFields,
        string $address = null,
        string $statusPath = '/status',
        $formats = ['plain', 'html', 'xml', 'json']
    ) {
        if (!is_array($formats)) {
            $formats = [$formats];
        }

        require_once "status.inc";
        $status = new Status();
        foreach ($formats as $format) {
            $query = $format === 'plain' ? '' : $format;
            $response = $this->request($query, [], $statusPath, $address);
            $status->checkStatus($response, $expectedFields, $format);
        }
    }

    /**
     * Execute request.
     *
     * @param string $query
     * @param array $headers
     * @param string|null $uri
     * @param string|null $address
     * @param string|null $successMessage
     * @param string|null $errorMessage
     * @param bool $connKeepAlive
     * @return Response
     */
    public function request(
        string $query = '',
        array $headers = [],
        string $uri = null,
        string $address = null,
        string $successMessage = null,
        string $errorMessage = null,
        bool $connKeepAlive = false
    ) {
        if ($this->hasError()) {
            return new Response(null, true);
        }
        if (is_null($uri)) {
            $uri = $this->makeFile('src.php', $this->code);
        }

        $params = array_merge(
            [
                'GATEWAY_INTERFACE' => 'FastCGI/1.0',
                'REQUEST_METHOD'    => 'GET',
                'SCRIPT_FILENAME'   => $uri,
                'SCRIPT_NAME'       => $uri,
                'QUERY_STRING'      => $query,
                'REQUEST_URI'       => $uri . ($query ? '?'.$query : ""),
                'DOCUMENT_URI'      => $uri,
                'SERVER_SOFTWARE'   => 'php/fcgiclient',
                'REMOTE_ADDR'       => '127.0.0.1',
                'REMOTE_PORT'       => '7777',
                'SERVER_ADDR'       => '127.0.0.1',
                'SERVER_PORT'       => '80',
                'SERVER_NAME'       => php_uname('n'),
                'SERVER_PROTOCOL'   => 'HTTP/1.1',
                'DOCUMENT_ROOT'     => __DIR__,
                'CONTENT_TYPE'      => '',
                'CONTENT_LENGTH'    => 0
            ],
            $headers
        );

        try {
            $this->response = new Response(
                $this->getClient($address, $connKeepAlive)->request_data($params, false)
            );
            $this->message($successMessage);
        } catch (\Exception $exception) {
            if ($errorMessage === null) {
                $this->error("Request failed", $exception);
            } else {
                $this->message($errorMessage);
            }
            $this->response = new Response();
        }
        if ($this->debug) {
            $this->response->debugOutput();
        }
        return $this->response;
    }

    /**
     * Get client.
     *
     * @param string $address
     * @param bool $keepAlive
     * @return Client
     */
    private function getClient(string $address = null, $keepAlive = false)
    {
        $address = $address ? $this->processTemplate($address) : $this->getAddr();
        if ($address[0] === '/') { // uds
            $host = 'unix://' . $address;
            $port = -1;
        } elseif ($address[0] === '[') { // ipv6
            $addressParts = explode(']:', $address);
            $host = $addressParts[0];
            if (isset($addressParts[1])) {
                $host .= ']';
                $port = $addressParts[1];
            } else {
                $port = $this->getPort();
            }
        } else { // ipv4
            $addressParts = explode(':', $address);
            $host = $addressParts[0];
            $port = $addressParts[1] ?? $this->getPort();
        }

        if (!$keepAlive) {
            return new Client($host, $port);
        }

        if (!isset($this->clients[$host][$port])) {
            $client = new Client($host, $port);
            $client->setKeepAlive(true);
            $this->clients[$host][$port] = $client;
        }

        return $this->clients[$host][$port];
    }

    /**
     * Display logs
     *
     * @param int $number
     * @param string $ignore
     */
    public function displayLog(int $number = 1, string $ignore = 'systemd')
    {
        /* Read $number lines or until EOF */
        while ($number > 0 || ($number < 0 && !feof($this->outDesc))) {
            $a = fgets($this->outDesc);
            if (empty($ignore) || !strpos($a, $ignore)) {
                echo $a;
                $number--;
            }
        }
    }

    /**
     * Get a single log line
     *
     * @return null|string
     */
    private function getLogLine()
    {
        $read = [$this->outDesc];
        $write = null;
        $except = null;
        if (stream_select($read, $write, $except, 2 )) {
            return fgets($this->outDesc);
        } else {
            return null;
        }
    }

    /**
     * Get log lines
     *
     * @param int $number
     * @param bool $skipBlank
     * @param string $ignore
     * @return array
     */
    public function getLogLines(int $number = 1, bool $skipBlank = false, string $ignore = 'systemd')
    {
        $lines = [];
        /* Read $n lines or until EOF */
        while ($number > 0 || ($number < 0 && !feof($this->outDesc))) {
            $line = $this->getLogLine();
            if (is_null($line)) {
                break;
            }
            if ((empty($ignore) || !strpos($line, $ignore)) && (!$skipBlank || strlen(trim($line)) > 0)) {
                $lines[] = $line;
                $number--;
            }
        }

        return $lines;
    }

    /**
     * @return mixed|string
     */
    public function getLastLogLine()
    {
        $lines = $this->getLogLines();

        return $lines[0] ?? '';
    }

    /**
     * Send signal to the supplied PID or the server PID.
     *
     * @param string $signal
     * @param int|null $pid
     * @return string
     */
    public function signal($signal, int $pid = null)
    {
        if (is_null($pid)) {
            $pid = $this->getPid();
        }

        return exec("kill -$signal $pid");
    }

    /**
     * Terminate master process
     */
    public function terminate()
    {
        proc_terminate($this->masterProcess);
    }

    /**
     * Close all open descriptors and process resources
     *
     * @param bool $terminate
     */
    public function close($terminate = false)
    {
        if ($terminate) {
            $this->terminate();
        }
        fclose($this->outDesc);
        proc_close($this->masterProcess);
    }

    /**
     * Create a config file.
     *
     * @param string $extension
     * @return string
     * @throws \Exception
     */
    private function createConfig($extension = 'ini')
    {
        if (is_array($this->configTemplate)) {
            $configTemplates = $this->configTemplate;
            if (!isset($configTemplates['main'])) {
                throw new \Exception('The config template array has to have main config');
            }
            $mainTemplate = $configTemplates['main'];
            unset($configTemplates['main']);
            if (!is_dir(self::CONF_DIR)) {
                mkdir(self::CONF_DIR);
            }
            foreach ($configTemplates as $name => $configTemplate) {
                $this->makeFile(
                    'conf',
                    $this->processTemplate($configTemplate),
                    self::CONF_DIR,
                    $name
                );
            }
        } else {
            $mainTemplate = $this->configTemplate;
        }

        return $this->makeFile($extension, $this->processTemplate($mainTemplate));
    }

    /**
     * Process template string.
     *
     * @param string $template
     * @return string
     */
    private function processTemplate(string $template)
    {
        $vars = [
            'FILE:LOG:ACC' => ['getAbsoluteFile', self::FILE_EXT_LOG_ACC],
            'FILE:LOG:ERR' => ['getAbsoluteFile', self::FILE_EXT_LOG_ERR],
            'FILE:LOG:SLOW' => ['getAbsoluteFile', self::FILE_EXT_LOG_SLOW],
            'FILE:PID' => ['getAbsoluteFile', self::FILE_EXT_PID],
            'RFILE:LOG:ACC' => ['getRelativeFile', self::FILE_EXT_LOG_ACC],
            'RFILE:LOG:ERR' => ['getRelativeFile', self::FILE_EXT_LOG_ERR],
            'RFILE:LOG:SLOW' => ['getRelativeFile', self::FILE_EXT_LOG_SLOW],
            'RFILE:PID' => ['getRelativeFile', self::FILE_EXT_PID],
            'ADDR:IPv4' => ['getAddr', 'ipv4'],
            'ADDR:IPv4:ANY' => ['getAddr', 'ipv4-any'],
            'ADDR:IPv6' => ['getAddr', 'ipv6'],
            'ADDR:IPv6:ANY' => ['getAddr', 'ipv6-any'],
            'ADDR:UDS' => ['getAddr', 'uds'],
            'PORT' => ['getPort', 'ip'],
            'INCLUDE:CONF' => self::CONF_DIR . '/*.conf',
        ];
        $aliases = [
            'ADDR' => 'ADDR:IPv4',
            'FILE:LOG' => 'FILE:LOG:ERR',
        ];
        foreach ($aliases as $aliasName => $aliasValue) {
            $vars[$aliasName] = $vars[$aliasValue];
        }

        return preg_replace_callback(
            '/{{([a-zA-Z0-9:]+)(\[\w+\])?}}/',
            function ($matches) use ($vars) {
                $varName = $matches[1];
                if (!isset($vars[$varName])) {
                    $this->error("Invalid config variable $varName");
                    return 'INVALID';
                }
                $pool = $matches[2] ?? 'default';
                $varValue = $vars[$varName];
                if (is_string($varValue)) {
                    return $varValue;
                }
                $functionName = array_shift($varValue);
                $varValue[] = $pool;
                return call_user_func_array([$this, $functionName], $varValue);
            },
            $template
        );
    }

    /**
     * @param string $type
     * @param string $pool
     * @return string
     */
    public function getAddr(string $type = 'ipv4', $pool = 'default')
    {
        $port = $this->getPort($type, $pool, true);
        if ($type === 'uds') {
            return $this->getFile($port . '.sock');
        }

        return $this->getHost($type) . ':' . $port;
    }

    /**
     * @param string $type
     * @param string $pool
     * @param bool $useAsId
     * @return int
     */
    public function getPort(string $type = 'ip', $pool = 'default', $useAsId = false)
    {
        if ($type === 'uds' && !$useAsId) {
            return -1;
        }

        if (isset($this->ports['values'][$pool])) {
            return $this->ports['values'][$pool];
        }
        $port = ($this->ports['last'] ?? 9000 + PHP_INT_SIZE - 1) + 1;
        $this->ports['values'][$pool] = $this->ports['last'] = $port;

        return $port;
    }

    /**
     * @param string $type
     * @return string
     */
    public function getHost(string $type = 'ipv4')
    {
        switch ($type) {
            case 'ipv6-any':
                return '[::]';
            case 'ipv6':
                return '[::1]';
            case 'ipv4-any':
                return '0.0.0.0';
            default:
                return '127.0.0.1';
        }
    }

    /**
     * Get listen address.
     *
     * @param string|null $template
     * @return string
     */
    public function getListen($template = null)
    {
        return $template ? $this->processTemplate($template) : $this->getAddr();
    }

    /**
     * Get PID.
     *
     * @return int
     */
    public function getPid()
    {
        $pidFile = $this->getFile('pid');
        if (!is_file($pidFile)) {
            return (int) $this->error("PID file has not been created");
        }
        $pidContent = file_get_contents($pidFile);
        if (!is_numeric($pidContent)) {
            return (int) $this->error("PID content '$pidContent' is not integer");
        }

        return (int) $pidContent;
    }


    /**
     * @param string $extension
     * @param string|null $dir
     * @param string|null $name
     * @return string
     */
    private function getFile(string $extension, $dir = null, $name = null)
    {
        $fileName = (is_null($name) ? $this->fileName : $name . '.') . $extension;

        return is_null($dir) ? $fileName : $dir . '/'  . $fileName;
    }

    /**
     * @param string $extension
     * @return string
     */
    private function getAbsoluteFile(string $extension)
    {
        return $this->getFile($extension);
    }

    /**
     * @param string $extension
     * @return string
     */
    private function getRelativeFile(string $extension)
    {
        $fileName = rtrim(basename($this->fileName), '.');

        return $this->getFile($extension, null, $fileName);
    }

    /**
     * @param string $extension
     * @param string $prefix
     * @return string
     */
    private function getPrefixedFile(string $extension, string $prefix = null)
    {
        $fileName = rtrim($this->fileName, '.');
        if (!is_null($prefix)) {
            $fileName = $prefix . '/' . basename($fileName);
        }

        return $this->getFile($extension, null, $fileName);
    }

    /**
     * @param string $extension
     * @param string $content
     * @param string|null $dir
     * @param string|null $name
     * @return string
     */
    private function makeFile(string $extension, string $content = '', $dir = null, $name = null)
    {
        $filePath = $this->getFile($extension, $dir, $name);
        file_put_contents($filePath, $content);

        return $filePath;
    }

    /**
     * @param string|null $msg
     */
    private function message($msg)
    {
        if ($msg !== null) {
            echo "$msg\n";
        }
    }

    /**
     * @param string $msg
     * @param \Exception|null $exception
     */
    private function error($msg, \Exception $exception = null)
    {
        $this->error =  'ERROR: ' . $msg;
        if ($exception) {
            $this->error .= '; EXCEPTION: ' . $exception->getMessage();
        }
        $this->error .= "\n";

        echo $this->error;
    }

    /**
     * @return bool
     */
    private function hasError()
    {
        return !is_null($this->error) || !is_null($this->logTool->getError());
    }

    /**
     * Expect file with a supplied extension to exist.
     *
     * @param string $extension
     * @param string $prefix
     * @return bool
     */
    public function expectFile(string $extension, $prefix = null)
    {
        $filePath = $this->getPrefixedFile($extension, $prefix);
        if (!file_exists($filePath)) {
            return $this->error("The file $filePath does not exist");
        }

        return true;
    }

    /**
     * Expect file with a supplied extension to not exist.
     *
     * @param string $extension
     * @param string $prefix
     * @return bool
     */
    public function expectNoFile(string $extension, $prefix = null)
    {
        $filePath = $this->getPrefixedFile($extension, $prefix);
        if (file_exists($filePath)) {
            return $this->error("The file $filePath exists");
        }

        return true;
    }

    /**
     * Expect message to be written to FastCGI error stream.
     *
     * @param string $message
     * @param int $limit
     * @param int $repeat
     */
    public function expectFastCGIErrorMessage(
        string $message,
        int $limit = 1024,
        int $repeat = 0
    ) {
        $this->logTool->setExpectedMessage($message, $limit, $repeat);
        $this->logTool->checkTruncatedMessage($this->response->getErrorData());
    }

    /**
     * Expect starting lines to be logged.
     */
    public function expectLogStartNotices()
    {
        $this->logTool->expectStartingLines($this->getLogLines(2));
    }

    /**
     * Expect terminating lines to be logged.
     */
    public function expectLogTerminatingNotices()
    {
        $this->logTool->expectTerminatorLines($this->getLogLines(-1));
    }

    /**
     * Expect log message that can span multiple lines.
     *
     * @param string $message
     * @param int $limit
     * @param int $repeat
     * @param bool $decorated
     * @param bool $wrapped
     */
    public function expectLogMessage(
        string $message,
        int $limit = 1024,
        int $repeat = 0,
        bool $decorated = true,
        bool $wrapped = true
    ) {
        $this->logTool->setExpectedMessage($message, $limit, $repeat);
        if ($wrapped) {
            $logLines = $this->getLogLines(-1, true);
            $this->logTool->checkWrappedMessage($logLines, true, $decorated);
        } else {
            $logLines = $this->getLogLines(1, true);
            $this->logTool->checkTruncatedMessage($logLines[0] ?? '');
        }
        if ($this->debug) {
            $this->message("-------------- LOG LINES: -------------");
            var_dump($logLines);
            $this->message("---------------------------------------\n");
        }
    }

    /**
     * Expect a single log line.
     *
     * @param string $message
     * @return bool
     */
    public function expectLogLine(string $message)
    {
        $messageLen = strlen($message);
        $limit = $messageLen > 1024 ? $messageLen + 16 : 1024;
        $this->logTool->setExpectedMessage($message, $limit);
        $logLines = $this->getLogLines(1, true);
        if ($this->debug) {
            $this->message("LOG LINE: " . ($logLines[0] ?? ''));
        }

        return $this->logTool->checkWrappedMessage($logLines, false);
    }

    /**
     * Expect a log debug message.
     *
     * @param string $message
     * @param string|null $pool
     * @return bool
     */
    public function expectLogDebug(string $message, $pool = null)
    {
        return $this->logTool->expectDebug($this->getLastLogLine(), $message, $pool);
    }

    /**
     * Expect a log notice.
     *
     * @param string $message
     * @param string|null $pool
     * @return bool
     */
    public function expectLogNotice(string $message, $pool = null)
    {
        return $this->logTool->expectNotice($this->getLastLogLine(), $message, $pool);
    }

    /**
     * Expect a log warning.
     *
     * @param string $message
     * @param string|null $pool
     * @return bool
     */
    public function expectLogWarning(string $message, $pool = null)
    {
        return $this->logTool->expectWarning($this->getLastLogLine(), $message, $pool);
    }

    /**
     * Expect a log error.
     *
     * @param string $message
     * @param string|null $pool
     * @return bool
     */
    public function expectLogError(string $message, $pool = null)
    {
        return $this->logTool->expectError($this->getLastLogLine(), $message, $pool);
    }

    /**
     * Expect a log alert.
     *
     * @param string $message
     * @param string|null $pool
     * @return bool
     */
    public function expectLogAlert(string $message, $pool = null)
    {
        return $this->logTool->expectAlert($this->getLastLogLine(), $message, $pool);
    }

    /**
     * Expect no log lines to be logged.
     *
     * @return bool
     */
    public function expectNoLogMessages()
    {
        $logLines = $this->getLogLines(-1, true);
        if (!empty($logLines)) {
            return $this->error(
                "Expected no log lines but following lines logged:\n" . implode("\n", $logLines)
            );
        }

        return true;
    }

    /**
     * Print content of access log.
     */
    public function printAccessLog()
    {
        $accessLog = $this->getFile('acc.log');
        if (is_file($accessLog)) {
            print file_get_contents($accessLog);
        }
    }
}
