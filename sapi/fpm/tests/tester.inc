<?php

namespace FPM;

use FPM\FastCGI\Client;

require_once 'fcgi.inc';
require_once 'logreader.inc';
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
    static private array $supportedFiles = [
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
    static private array $filesToClean = ['.user.ini'];

    /**
     * @var bool
     */
    private bool $debug;

    /**
     * @var array
     */
    private array $clients = [];

    /**
     * @var LogReader
     */
    private LogReader $logReader;

    /**
     * @var LogTool
     */
    private LogTool $logTool;

    /**
     * Configuration template
     *
     * @var string|array
     */
    private string|array $configTemplate;

    /**
     * The PHP code to execute
     *
     * @var string
     */
    private string $code;

    /**
     * @var array
     */
    private array $options;

    /**
     * @var string
     */
    private string $fileName;

    /**
     * @var resource
     */
    private $masterProcess;

    /**
     * @var bool
     */
    private bool $daemonized;

    /**
     * @var resource
     */
    private $outDesc;

    /**
     * @var array
     */
    private array $ports = [];

    /**
     * @var string|null
     */
    private ?string $error = null;

    /**
     * The last response for the request call
     *
     * @var Response|null
     */
    private ?Response $response;

    /**
     * @var string[]
     */
    private $expectedAccessLogs;

    /**
     * @var bool
     */
    private $expectSuppressableAccessLogEntries;

    /**
     * Clean all the created files up
     *
     * @param int $backTraceIndex
     */
    static public function clean($backTraceIndex = 1)
    {
        $filePrefix = self::getCallerFileName($backTraceIndex);
        if (str_ends_with($filePrefix, 'clean.')) {
            $filePrefix = substr($filePrefix, 0, -6);
        }

        $filesToClean = array_merge(
            array_map(
                function ($fileExtension) use ($filePrefix) {
                    return $filePrefix . $fileExtension;
                },
                self::$supportedFiles
            ),
            array_map(
                function ($fileExtension) {
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

        self::cleanConfigFiles();
    }

    /**
     * Clean config files
     */
    static public function cleanConfigFiles()
    {
        if (is_dir(self::CONF_DIR)) {
            foreach (glob(self::CONF_DIR . '/*.conf') as $name) {
                unlink($name);
            }
            rmdir(self::CONF_DIR);
        }
    }

    /**
     * @param int $backTraceIndex
     *
     * @return string
     */
    static private function getCallerFileName(int $backTraceIndex = 1): string
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
    static public function findExecutable(): bool|string
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
            if (file_exists($phpPath . "/fpm/php-fpm") && is_executable($phpPath . "/fpm/php-fpm")) {
                /* gotcha */
                return $phpPath . "/fpm/php-fpm";
            }
            $phpSbinFpmi = $phpPath . "/sbin/php-fpm";
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
        if ( ! is_array($files)) {
            $files = array($files);
        }
        foreach ($files as $file) {
            if ( ! file_exists($file)) {
                die("skip File $file does not exist");
            }
        }
    }

    /**
     * Skip test if config file is invalid.
     *
     * @param string $configTemplate
     *
     * @throws \Exception
     */
    static public function skipIfConfigFails(string $configTemplate)
    {
        $tester     = new self($configTemplate, '', [], self::getCallerFileName());
        $testResult = $tester->testConfig(true);
        if ($testResult !== null) {
            self::clean(2);
            $message = $testResult[0] ?? 'Config failed';
            die("skip $message");
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
     * Skip if not running as root.
     */
    static public function skipIfNotRoot()
    {
        if (exec('whoami') !== 'root') {
            die('skip not running as root');
        }
    }

    /**
     * Skip if running as root.
     */
    static public function skipIfRoot()
    {
        if (exec('whoami') === 'root') {
            die('skip running as root');
        }
    }

    /**
     * Skip if posix extension not loaded.
     */
    static public function skipIfPosixNotLoaded()
    {
        if ( ! extension_loaded('posix')) {
            die('skip posix extension not loaded');
        }
    }

    /**
     * Skip test if supplied shell command fails.
     *
     * @param string $command
     */
    static public function skipIfShellCommandFails($command)
    {
        $output = system($command, $code);
        if ($code) {
            die("skip command '$command' faield with code $code and output: $output");
        }
    }

    /**
     * Skip if posix extension not loaded.
     */
    static public function skipIfUserDoesNotExist($userName)
    {
        self::skipIfPosixNotLoaded();
        if (posix_getpwnam($userName) === false) {
            die("skip user $userName does not exist");
        }
    }

    /**
     * Tester constructor.
     *
     * @param string|array $configTemplate
     * @param string       $code
     * @param array        $options
     * @param string|null  $fileName
     * @param bool|null    $debug
     */
    public function __construct(
        string|array $configTemplate,
        string $code = '',
        array $options = [],
        string $fileName = null,
        bool $debug = null
    ) {
        $this->configTemplate = $configTemplate;
        $this->code           = $code;
        $this->options        = $options;
        $this->fileName       = $fileName ?: self::getCallerFileName();
        $this->debug          = $debug !== null ? $debug : (bool)getenv('TEST_FPM_DEBUG');
        $this->logReader      = new LogReader($this->debug);
        $this->logTool        = new LogTool($this->logReader, $this->debug);
    }

    /**
     * @param string $ini
     */
    public function setUserIni(string $ini)
    {
        $iniFile = __DIR__ . '/.user.ini';
        $this->trace('Setting .user.ini file', $ini, isFile: true);
        file_put_contents($iniFile, $ini);
    }

    /**
     * Test configuration file.
     *
     * @return null|array
     * @throws \Exception
     */
    public function testConfig($silent = false, array|string|null $expectedPattern = null): ?array
    {
        $configFile = $this->createConfig();
        $cmd        = self::findExecutable() . ' -n -tt -y ' . $configFile . ' 2>&1';
        $this->trace('Testing config using command', $cmd, true);
        exec($cmd, $output, $code);
        $found = 0;
        if ($expectedPattern !== null) {
            $expectedPatterns = is_array($expectedPattern) ? $expectedPattern : [$expectedPattern];
        }
        if ($code) {
            $messages = [];
            foreach ($output as $outputLine) {
                $message    = preg_replace("/\[.+?\]/", "", $outputLine, 1);
                if ($expectedPattern !== null) {
                    for ($i = 0; $i < count($expectedPatterns); $i++) {
                        $pattern = $expectedPatterns[$i];
                        if ($pattern !== null && preg_match($pattern, $message)) {
                            $found++;
                            $expectedPatterns[$i] = null;
                        }
                    }
                }
                $messages[] = $message;
                if ( ! $silent) {
                    $this->error($message, null, false);
                }
            }
        } else {
            $messages = null;
        }

        if ($expectedPattern !== null && $found < count($expectedPatterns)) {
            $missingPatterns = array_filter($expectedPatterns);
            $errorMessage = sprintf(
                "The expected config %s %s %s not been found",
                count($missingPatterns) > 1 ? 'patterns' : 'pattern',
                implode(', ', $missingPatterns),
                count($missingPatterns) > 1 ? 'have' : 'has',
            );
            $this->error($errorMessage);
        }

        return $messages;
    }

    /**
     * Start PHP-FPM master process
     *
     * @param array $extraArgs   Command extra arguments.
     * @param bool  $forceStderr Whether to output to stderr so error log is used.
     * @param bool  $daemonize   Whether to start FPM daemonized
     * @param array $extensions  List of extension to add if shared build used.
     * @param array $iniEntries  List of ini entries to use.
     *
     * @return bool
     * @throws \Exception
     */
    public function start(
        array $extraArgs = [],
        bool $forceStderr = true,
        bool $daemonize = false,
        array $extensions = [],
        array $iniEntries = [],
    ) {
        $configFile = $this->createConfig();
        $desc       = $this->outDesc ? [] : [1 => array('pipe', 'w'), 2 => array('redirect', 1)];

        $cmd = [self::findExecutable(), '-n', '-y', $configFile];

        if ($forceStderr) {
            $cmd[] = '-O';
        }

        $this->daemonized = $daemonize;
        if ( ! $daemonize) {
            $cmd[] = '-F';
        }

        $extensionDir = getenv('TEST_FPM_EXTENSION_DIR');
        if ($extensionDir) {
            $cmd[] = '-dextension_dir=' . $extensionDir;
            foreach ($extensions as $extension) {
                $cmd[] = '-dextension=' . $extension;
            }
        }

        foreach ($iniEntries as $iniEntryName => $iniEntryValue) {
            $cmd[] = '-d' . $iniEntryName . '=' . $iniEntryValue;
        }

        if (getenv('TEST_FPM_RUN_AS_ROOT')) {
            $cmd[] = '--allow-to-run-as-root';
        }
        $cmd = array_merge($cmd, $extraArgs);
        $this->trace('Starting FPM using command:', $cmd, true);

        $this->masterProcess = proc_open($cmd, $desc, $pipes);
        register_shutdown_function(
            function ($masterProcess) use ($configFile) {
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
        if ( ! $this->outDesc !== false) {
            $this->outDesc = $pipes[1];
            $this->logReader->setStreamSource('{{MASTER:OUT}}', $this->outDesc);
            if ($daemonize) {
                $this->switchLogSource('{{FILE:LOG}}');
            }
        }

        return true;
    }

    /**
     * Run until needle is found in the log.
     *
     * @param string $pattern Search pattern to find.
     *
     * @return bool
     * @throws \Exception
     */
    public function runTill(string $pattern)
    {
        $this->start();
        $found = $this->logTool->expectPattern($pattern);
        $this->close(true);

        return $found;
    }

    /**
     * Check if connection works.
     *
     * @param string      $host
     * @param string|null $successMessage
     * @param string|null $errorMessage
     * @param int         $attempts
     * @param int         $delay
     */
    public function checkConnection(
        string $host = '127.0.0.1',
        string $successMessage = null,
        ?string $errorMessage = 'Connection failed',
        int $attempts = 20,
        int $delay = 50000
    ) {
        $i = 0;
        do {
            if ($i > 0 && $delay > 0) {
                usleep($delay);
            }
            $fp = @fsockopen($host, $this->getPort());
        } while ((++$i < $attempts) && ! $fp);

        if ($fp) {
            $this->trace('Checking connection successful');
            $this->message($successMessage);
            fclose($fp);
        } else {
            $this->message($errorMessage);
        }
    }


    /**
     * Execute request with parameters ordered for better checking.
     *
     * @param string      $address
     * @param string|null $successMessage
     * @param string|null $errorMessage
     * @param string      $uri
     * @param string      $query
     * @param array       $headers
     *
     * @return Response
     */
    public function checkRequest(
        string $address,
        string $successMessage = null,
        string $errorMessage = null,
        string $uri = '/ping',
        string $query = '',
        array $headers = []
    ): Response {
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
     * @param array       $expectedFields
     * @param string|null $address
     * @param string      $statusPath
     * @param mixed       $formats
     *
     * @throws \Exception
     */
    public function status(
        array $expectedFields,
        string $address = null,
        string $statusPath = '/status',
        $formats = ['plain', 'html', 'xml', 'json', 'openmetrics']
    ) {
        if ( ! is_array($formats)) {
            $formats = [$formats];
        }

        require_once "status.inc";
        $status = new Status();
        foreach ($formats as $format) {
            $query    = $format === 'plain' ? '' : $format;
            $response = $this->request($query, [], $statusPath, $address);
            $status->checkStatus($response, $expectedFields, $format);
        }
    }

    /**
     * Get request params array.
     *
     * @param string      $query
     * @param array       $headers
     * @param string|null $uri
     * @param string|null $scriptFilename
     * @param string|null $stdin
     *
     * @return array
     */
    private function getRequestParams(
        string $query = '',
        array $headers = [],
        string $uri = null,
        string $scriptFilename = null,
        string $scriptName = null,
        ?string $stdin = null
    ): array {
        if (is_null($scriptFilename)) {
            $scriptFilename = $this->makeSourceFile();
        }
        if (is_null($uri)) {
            $uri = '/' . basename($scriptFilename);
        }
        if (is_null($scriptName)) {
            $scriptName = $uri;
        }

        $params = array_merge(
            [
                'GATEWAY_INTERFACE' => 'FastCGI/1.0',
                'REQUEST_METHOD'    => is_null($stdin) ? 'GET' : 'POST',
                'SCRIPT_FILENAME'   => $scriptFilename === '' ? null : $scriptFilename,
                'SCRIPT_NAME'       => $scriptName,
                'QUERY_STRING'      => $query,
                'REQUEST_URI'       => $uri . ($query ? '?' . $query : ""),
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
                'CONTENT_LENGTH'    => strlen($stdin ?? "") // Default to 0
            ],
            $headers
        );

        return array_filter($params, function ($value) {
            return ! is_null($value);
        });
    }

    /**
     * Parse stdin and generate data for multipart config.
     *
     * @param array $stdin
     * @param array $headers
     *
     * @return void
     * @throws \Exception
     */
    private function parseStdin(array $stdin, array &$headers)
    {
        $parts = $stdin['parts'] ?? null;
        if (empty($parts)) {
            throw new \Exception('The stdin array needs to contain parts');
        }
        $boundary = $stdin['boundary'] ?? 'AaB03x';
        if ( ! isset($headers['CONTENT_TYPE'])) {
            $headers['CONTENT_TYPE'] = 'multipart/form-data; boundary=' . $boundary;
        }
        $count = $parts['count'] ?? null;
        if ( ! is_null($count)) {
            $dispositionType  = $parts['disposition'] ?? 'form-data';
            $dispositionParam = $parts['param'] ?? 'name';
            $namePrefix       = $parts['prefix'] ?? 'f';
            $nameSuffix       = $parts['suffix'] ?? '';
            $value            = $parts['value'] ?? 'test';
            $parts            = [];
            for ($i = 0; $i < $count; $i++) {
                $parts[] = [
                    'disposition' => $dispositionType,
                    'param'       => $dispositionParam,
                    'name'        => "$namePrefix$i$nameSuffix",
                    'value'       => $value
                ];
            }
        }
        $out = '';
        $nl  = "\r\n";
        foreach ($parts as $part) {
            if (!is_array($part)) {
                $part = ['name' => $part];
            } elseif ( ! isset($part['name'])) {
                throw new \Exception('Each part has to have a name');
            }
            $name             = $part['name'];
            $dispositionType  = $part['disposition'] ?? 'form-data';
            $dispositionParam = $part['param'] ?? 'name';
            $value            = $part['value'] ?? 'test';
            $partHeaders          = $part['headers'] ?? [];

            $out .= "--$boundary$nl";
            $out .= "Content-disposition: $dispositionType; $dispositionParam=\"$name\"$nl";
            foreach ($partHeaders as $headerName => $headerValue) {
                $out .= "$headerName: $headerValue$nl";
            }
            $out .= $nl;
            $out .= "$value$nl";
        }
        $out .= "--$boundary--$nl";

        return $out;
    }

    /**
     * Execute request.
     *
     * @param string            $query
     * @param array             $headers
     * @param string|null       $uri
     * @param string|null       $address
     * @param string|null       $successMessage
     * @param string|null       $errorMessage
     * @param bool              $connKeepAlive
     * @param string|null       $scriptFilename = null
     * @param string|null       $scriptName = null
     * @param string|array|null $stdin          = null
     * @param bool              $expectError
     * @param int               $readLimit
     * @param int               $writeDelay
     *
     * @return Response
     * @throws \Exception
     */
    public function request(
        string $query = '',
        array $headers = [],
        string $uri = null,
        string $address = null,
        string $successMessage = null,
        string $errorMessage = null,
        bool $connKeepAlive = false,
        string $scriptFilename = null,
        string $scriptName = null,
        string|array $stdin = null,
        bool $expectError = false,
        int $readLimit = -1,
        int $writeDelay = 0,
    ): Response {
        if ($this->hasError()) {
            return new Response(null, true);
        }

        if (is_array($stdin)) {
            $stdin = $this->parseStdin($stdin, $headers);
        }

        $params = $this->getRequestParams($query, $headers, $uri, $scriptFilename, $scriptName, $stdin);
        $this->trace('Request params', $params);

        try {
            $this->response = new Response(
                $this->getClient($address, $connKeepAlive)->request_data($params, $stdin, $readLimit, $writeDelay)
            );
            if ($expectError) {
                $this->error('Expected request error but the request was successful');
            } else {
                $this->message($successMessage);
            }
        } catch (\Exception $exception) {
            if ($expectError) {
                $this->message($successMessage);
            } elseif ($errorMessage === null) {
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
     * Execute multiple requests in parallel.
     *
     * @param int|array   $requests
     * @param string|null $address
     * @param string|null $successMessage
     * @param string|null $errorMessage
     * @param bool        $connKeepAlive
     * @param int         $readTimeout
     * @param int         $writeDelay
     *
     * @return Response[]
     * @throws \Exception
     */
    public function multiRequest(
        int|array $requests,
        string $address = null,
        string $successMessage = null,
        string $errorMessage = null,
        bool $connKeepAlive = false,
        int $readTimeout = 0,
        int $writeDelay = 0,
    ) {
        if (is_numeric($requests)) {
            $requests = array_fill(0, $requests, []);
        }

        if ($this->hasError()) {
            return array_map(fn($request) => new Response(null, true), $requests);
        }

        try {
            $connections = array_map(function ($requestData) use ($address, $connKeepAlive, $writeDelay) {
                $client = $this->getClient($address, $connKeepAlive);
                $params = $this->getRequestParams(
                    $requestData['query'] ?? '',
                    $requestData['headers'] ?? [],
                    $requestData['uri'] ?? null
                );

                if (isset($requestData['delay'])) {
                    usleep($requestData['delay']);
                }

                return [
                    'client'    => $client,
                    'requestId' => $client->async_request($params, false, $writeDelay),
                ];
            }, $requests);

            $responses = array_map(function ($conn) use ($readTimeout) {
                $response = new Response($conn['client']->wait_for_response_data($conn['requestId'], $readTimeout));
                if ($this->debug) {
                    $response->debugOutput();
                }

                return $response;
            }, $connections);
            $this->message($successMessage);

            return $responses;
        } catch (\Exception $exception) {
            if ($errorMessage === null) {
                $this->error("Request failed", $exception);
            } else {
                $this->message($errorMessage);
            }

            return array_map(fn($request) => new Response(null, true), $requests);
        }
    }

    /**
     * Get client.
     *
     * @param string $address
     * @param bool   $keepAlive
     *
     * @return Client
     */
    private function getClient(string $address = null, $keepAlive = false): Client
    {
        $address = $address ? $this->processTemplate($address) : $this->getAddr();
        if ($address[0] === '/') { // uds
            $host = 'unix://' . $address;
            $port = -1;
        } elseif ($address[0] === '[') { // ipv6
            $addressParts = explode(']:', $address);
            $host         = $addressParts[0];
            if (isset($addressParts[1])) {
                $host .= ']';
                $port = $addressParts[1];
            } else {
                $port = $this->getPort();
            }
        } else { // ipv4
            $addressParts = explode(':', $address);
            $host         = $addressParts[0];
            $port         = $addressParts[1] ?? $this->getPort();
        }

        if ( ! $keepAlive) {
            return new Client($host, $port);
        }

        if ( ! isset($this->clients[$host][$port])) {
            $client = new Client($host, $port);
            $client->setKeepAlive(true);
            $this->clients[$host][$port] = $client;
        }

        return $this->clients[$host][$port];
    }

    /**
     * @return string
     */
    public function getUser()
    {
        return get_current_user();
    }

    /**
     * @return string
     */
    public function getGroup()
    {
        return get_current_group();
    }

    /**
     * @return int
     */
    public function getUid()
    {
        return getmyuid();
    }

    /**
     * @return int
     */
    public function getGid()
    {
        return getmygid();
    }

    /**
     * Reload FPM by sending USR2 signal and optionally change config before that.
     *
     * @param string|array $configTemplate
     *
     * @return string
     * @throws \Exception
     */
    public function reload($configTemplate = null)
    {
        if ( ! is_null($configTemplate)) {
            self::cleanConfigFiles();
            $this->configTemplate = $configTemplate;
            $this->createConfig();
        }

        return $this->signal('USR2');
    }

    /**
     * Reload FPM logs by sending USR1 signal.
     *
     * @return string
     * @throws \Exception
     */
    public function reloadLogs(): string
    {
        return $this->signal('USR1');
    }

    /**
     * Send signal to the supplied PID or the server PID.
     *
     * @param string   $signal
     * @param int|null $pid
     *
     * @return string
     */
    public function signal($signal, int $pid = null)
    {
        if (is_null($pid)) {
            $pid = $this->getPid();
        }
        $cmd = "kill -$signal $pid";
        $this->trace('Sending signal using command', $cmd, true);

        return exec("kill -$signal $pid");
    }

    /**
     * Terminate master process
     */
    public function terminate()
    {
        if ($this->daemonized) {
            $this->signal('TERM');
        } else {
            proc_terminate($this->masterProcess);
        }
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
        proc_close($this->masterProcess);
    }

    /**
     * Create a config file.
     *
     * @param string $extension
     *
     * @return string
     * @throws \Exception
     */
    private function createConfig($extension = 'ini')
    {
        if (is_array($this->configTemplate)) {
            $configTemplates = $this->configTemplate;
            if ( ! isset($configTemplates['main'])) {
                throw new \Exception('The config template array has to have main config');
            }
            $mainTemplate = $configTemplates['main'];
            if ( ! is_dir(self::CONF_DIR)) {
                mkdir(self::CONF_DIR);
            }
            foreach ($this->createPoolConfigs($configTemplates) as $name => $poolConfig) {
                $this->makeFile(
                    'conf',
                    $this->processTemplate($poolConfig),
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
     * Create pool config templates.
     *
     * @param array $configTemplates
     *
     * @return array
     * @throws \Exception
     */
    private function createPoolConfigs(array $configTemplates)
    {
        if ( ! isset($configTemplates['poolTemplate'])) {
            unset($configTemplates['main']);

            return $configTemplates;
        }
        $poolTemplate = $configTemplates['poolTemplate'];
        $configs      = [];
        if (isset($configTemplates['count'])) {
            $start = $configTemplates['start'] ?? 1;
            for ($i = $start; $i < $start + $configTemplates['count']; $i++) {
                $configs[$i] = str_replace('%index%', $i, $poolTemplate);
            }
        } elseif (isset($configTemplates['names'])) {
            foreach ($configTemplates['names'] as $name) {
                $configs[$name] = str_replace('%name%', $name, $poolTemplate);
            }
        } else {
            throw new \Exception('The config template requires count or names if poolTemplate set');
        }

        return $configs;
    }

    /**
     * Process template string.
     *
     * @param string $template
     *
     * @return string
     */
    private function processTemplate(string $template)
    {
        $vars    = [
            'FILE:LOG:ACC'   => ['getAbsoluteFile', self::FILE_EXT_LOG_ACC],
            'FILE:LOG:ERR'   => ['getAbsoluteFile', self::FILE_EXT_LOG_ERR],
            'FILE:LOG:SLOW'  => ['getAbsoluteFile', self::FILE_EXT_LOG_SLOW],
            'FILE:PID'       => ['getAbsoluteFile', self::FILE_EXT_PID],
            'RFILE:LOG:ACC'  => ['getRelativeFile', self::FILE_EXT_LOG_ACC],
            'RFILE:LOG:ERR'  => ['getRelativeFile', self::FILE_EXT_LOG_ERR],
            'RFILE:LOG:SLOW' => ['getRelativeFile', self::FILE_EXT_LOG_SLOW],
            'RFILE:PID'      => ['getRelativeFile', self::FILE_EXT_PID],
            'ADDR:IPv4'      => ['getAddr', 'ipv4'],
            'ADDR:IPv4:ANY'  => ['getAddr', 'ipv4-any'],
            'ADDR:IPv6'      => ['getAddr', 'ipv6'],
            'ADDR:IPv6:ANY'  => ['getAddr', 'ipv6-any'],
            'ADDR:UDS'       => ['getAddr', 'uds'],
            'PORT'           => ['getPort', 'ip'],
            'INCLUDE:CONF'   => self::CONF_DIR . '/*.conf',
            'USER'           => ['getUser'],
            'GROUP'          => ['getGroup'],
            'UID'            => ['getUid'],
            'GID'            => ['getGid'],
            'MASTER:OUT'     => 'pipe:1',
            'STDERR'         => '/dev/stderr',
            'STDOUT'         => '/dev/stdout',
        ];
        $aliases = [
            'ADDR'     => 'ADDR:IPv4',
            'FILE:LOG' => 'FILE:LOG:ERR',
        ];
        foreach ($aliases as $aliasName => $aliasValue) {
            $vars[$aliasName] = $vars[$aliasValue];
        }

        return preg_replace_callback(
            '/{{([a-zA-Z0-9:]+)(\[\w+\])?}}/',
            function ($matches) use ($vars) {
                $varName = $matches[1];
                if ( ! isset($vars[$varName])) {
                    $this->error("Invalid config variable $varName");

                    return 'INVALID';
                }
                $pool     = $matches[2] ?? 'default';
                $varValue = $vars[$varName];
                if (is_string($varValue)) {
                    return $varValue;
                }
                $functionName = array_shift($varValue);
                $varValue[]   = $pool;

                return call_user_func_array([$this, $functionName], $varValue);
            },
            $template
        );
    }

    /**
     * @param string $type
     * @param string $pool
     *
     * @return string
     */
    public function getAddr(string $type = 'ipv4', $pool = 'default')
    {
        $port = $this->getPort($type, $pool, true);
        if ($type === 'uds') {
            $address = $this->getFile($port . '.sock');

            // Socket max path length is 108 on Linux and 104 on BSD,
            // so we use the latter
            if (strlen($address) <= 104) {
                return $address;
            }

            $addressPart = hash('crc32', dirname($address)) . '-' . basename($address);

            // is longer on Mac, than on Linux
            $tmpDirAddress = sys_get_temp_dir() . '/' . $addressPart;
                   ;

            if (strlen($tmpDirAddress) <= 104) {
                return $tmpDirAddress;
            }

            $srcRootAddress = dirname(__DIR__, 3) . '/' . $addressPart;

            return $srcRootAddress;
        }

        return $this->getHost($type) . ':' . $port;
    }

    /**
     * @param string $type
     * @param string $pool
     * @param bool   $useAsId
     *
     * @return int
     */
    public function getPort(string $type = 'ip', $pool = 'default', $useAsId = false)
    {
        if ($type === 'uds' && ! $useAsId) {
            return -1;
        }

        if (isset($this->ports['values'][$pool])) {
            return $this->ports['values'][$pool];
        }
        $port                         = ($this->ports['last'] ?? 9000 + PHP_INT_SIZE - 1) + 1;
        $this->ports['values'][$pool] = $this->ports['last'] = $port;

        return $port;
    }

    /**
     * @param string $type
     *
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
     *
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
        if ( ! is_file($pidFile)) {
            return (int)$this->error("PID file has not been created");
        }
        $pidContent = file_get_contents($pidFile);
        if ( ! is_numeric($pidContent)) {
            return (int)$this->error("PID content '$pidContent' is not integer");
        }
        $this->trace('PID found', $pidContent);

        return (int)$pidContent;
    }


    /**
     * Get file path for resource file.
     *
     * @param string      $extension
     * @param string|null $dir
     * @param string|null $name
     *
     * @return string
     */
    private function getFile(string $extension, string $dir = null, string $name = null): string
    {
        $fileName = (is_null($name) ? $this->fileName : $name . '.') . $extension;

        return is_null($dir) ? $fileName : $dir . '/' . $fileName;
    }

    /**
     * Get absolute file path for the resource file used by templates.
     *
     * @param string $extension
     *
     * @return string
     */
    private function getAbsoluteFile(string $extension): string
    {
        return $this->getFile($extension);
    }

    /**
     * Get relative file name for resource file used by templates.
     *
     * @param string $extension
     *
     * @return string
     */
    private function getRelativeFile(string $extension): string
    {
        $fileName = rtrim(basename($this->fileName), '.');

        return $this->getFile($extension, null, $fileName);
    }

    /**
     * Get prefixed file.
     *
     * @param string      $extension
     * @param string|null $prefix
     *
     * @return string
     */
    public function getPrefixedFile(string $extension, string $prefix = null): string
    {
        $fileName = rtrim($this->fileName, '.');
        if ( ! is_null($prefix)) {
            $fileName = $prefix . '/' . basename($fileName);
        }

        return $this->getFile($extension, null, $fileName);
    }

    /**
     * Create a resource file.
     *
     * @param string      $extension
     * @param string      $content
     * @param string|null $dir
     * @param string|null $name
     *
     * @return string
     */
    private function makeFile(
        string $extension,
        string $content = '',
        string $dir = null,
        string $name = null,
        bool $overwrite = true
    ): string {
        $filePath = $this->getFile($extension, $dir, $name);
        if ( ! $overwrite && is_file($filePath)) {
            return $filePath;
        }
        file_put_contents($filePath, $content);

        $this->trace('Created file: ' . $filePath, $content, isFile: true);

        return $filePath;
    }

    /**
     * Create a source code file.
     *
     * @return string
     */
    public function makeSourceFile(): string
    {
        return $this->makeFile('src.php', $this->code, overwrite: false);
    }

    /**
     * Create a source file and script name.
     *
     * @return string[]
     */
    public function createSourceFileAndScriptName(): array
    {
        $sourceFile = $this->makeFile('src.php', $this->code, overwrite: false);

        return [$sourceFile, '/' . basename($sourceFile)];
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
     * Display error.
     *
     * @param string          $msg       Error message.
     * @param \Exception|null $exception If there is an exception, log its message
     * @param bool            $prefix    Whether to prefix the error message
     *
     * @return false
     */
    private function error(string $msg, \Exception $exception = null, bool $prefix = true): bool
    {
        $this->error = $prefix ? 'ERROR: ' . $msg : ltrim($msg);
        if ($exception) {
            $this->error .= '; EXCEPTION: ' . $exception->getMessage();
        }
        $this->error .= "\n";

        echo $this->error;

        return false;
    }

    /**
     * Check whether any error was set.
     *
     * @return bool
     */
    private function hasError()
    {
        return ! is_null($this->error) || ! is_null($this->logTool->getError());
    }

    /**
     * Expect file with a supplied extension to exist.
     *
     * @param string $extension
     * @param string $prefix
     *
     * @return bool
     */
    public function expectFile(string $extension, $prefix = null)
    {
        $filePath = $this->getPrefixedFile($extension, $prefix);
        if ( ! file_exists($filePath)) {
            return $this->error("The file $filePath does not exist");
        }
        $this->trace('File path exists as expected', $filePath);

        return true;
    }

    /**
     * Expect file with a supplied extension to not exist.
     *
     * @param string $extension
     * @param string $prefix
     *
     * @return bool
     */
    public function expectNoFile(string $extension, $prefix = null)
    {
        $filePath = $this->getPrefixedFile($extension, $prefix);
        if (file_exists($filePath)) {
            return $this->error("The file $filePath exists");
        }
        $this->trace('File path does not exist as expected', $filePath);

        return true;
    }

    /**
     * Expect message to be written to FastCGI error stream.
     *
     * @param string $message
     * @param int    $limit
     * @param int    $repeat
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
     * Expect log to be empty.
     *
     * @throws \Exception
     */
    public function expectLogEmpty()
    {
        try {
            $line = $this->logReader->getLine(1, 0, true);
            if ($line === '') {
                $line = $this->logReader->getLine(1, 0, true);
            }
            if ($line !== null) {
                $this->error('Log is not closed and returned line: ' . $line);
            }
        } catch (LogTimoutException $exception) {
            $this->error('Log is not closed and timed out', $exception);
        }
    }

    /**
     * Expect reloading lines to be logged.
     *
     * @param int  $socketCount
     * @param bool $expectInitialProgressMessage
     * @param bool $expectReloadingMessage
     *
     * @throws \Exception
     */
    public function expectLogReloadingNotices(
        int $socketCount = 1,
        bool $expectInitialProgressMessage = true,
        bool $expectReloadingMessage = true
    ) {
        $this->logTool->expectReloadingLines(
            $socketCount,
            $expectInitialProgressMessage,
            $expectReloadingMessage
        );
    }

    /**
     * Expect reloading lines to be logged.
     *
     * @throws \Exception
     */
    public function expectLogReloadingLogsNotices()
    {
        $this->logTool->expectReloadingLogsLines();
    }

    /**
     * Expect starting lines to be logged.
     * @throws \Exception
     */
    public function expectLogStartNotices()
    {
        $this->logTool->expectStartingLines();
    }

    /**
     * Expect terminating lines to be logged.
     * @throws \Exception
     */
    public function expectLogTerminatingNotices()
    {
        $this->logTool->expectTerminatorLines();
    }

    /**
     * Expect log pattern in logs.
     *
     * @param string   $pattern             Log pattern
     * @param bool     $checkAllLogs        Whether to also check past logs.
     * @param int|null $timeoutSeconds      Timeout in seconds for reading of all messages.
     * @param int|null $timeoutMicroseconds Additional timeout in microseconds for reading of all messages.
     *
     * @throws \Exception
     */
    public function expectLogPattern(
        string $pattern,
        bool $checkAllLogs = false,
        int $timeoutSeconds = null,
        int $timeoutMicroseconds = null,
    ) {
        $this->logTool->expectPattern(
            $pattern,
            false,
            $checkAllLogs,
            $timeoutSeconds,
            $timeoutMicroseconds
        );
    }

    /**
     * Expect no such log pattern in logs.
     *
     * @param string   $pattern             Log pattern
     * @param bool     $checkAllLogs        Whether to also check past logs.
     * @param int|null $timeoutSeconds      Timeout in seconds for reading of all messages.
     * @param int|null $timeoutMicroseconds Additional timeout in microseconds for reading of all messages.
     *
     * @throws \Exception
     */
    public function expectNoLogPattern(
        string $pattern,
        bool $checkAllLogs = true,
        int $timeoutSeconds = null,
        int $timeoutMicroseconds = null,
    ) {
        if (is_null($timeoutSeconds) && is_null($timeoutMicroseconds)) {
            $timeoutMicroseconds = 10;
        }
        $this->logTool->expectPattern(
            $pattern,
            true,
            $checkAllLogs,
            $timeoutSeconds,
            $timeoutMicroseconds
        );
    }

    /**
     * Expect log message that can span multiple lines.
     *
     * @param string $message
     * @param int    $limit
     * @param int    $repeat
     * @param bool   $decorated
     * @param bool   $wrapped
     *
     * @throws \Exception
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
            $this->logTool->checkWrappedMessage(true, $decorated);
        } else {
            $this->logTool->checkTruncatedMessage();
        }
    }

    /**
     * Expect a single log line.
     *
     * @param string $message   The expected message.
     * @param bool   $isStdErr  Whether it is logged to stderr.
     * @param bool   $decorated Whether the log lines are decorated.
     *
     * @return bool
     * @throws \Exception
     */
    public function expectLogLine(
        string $message,
        bool $isStdErr = true,
        bool $decorated = true
    ): bool {
        $messageLen = strlen($message);
        $limit      = $messageLen > 1024 ? $messageLen + 16 : 1024;
        $this->logTool->setExpectedMessage($message, $limit);

        return $this->logTool->checkWrappedMessage(false, $decorated, $isStdErr);
    }

    /**
     * Expect log entry.
     *
     * @param string      $type                The log type.
     * @param string      $message             The expected message.
     * @param string|null $pool                The pool for pool prefixed log entry.
     * @param int         $count               The number of items.
     * @param bool        $checkAllLogs        Whether to also check past logs.
     * @param bool        $invert              Whether the log entry is not expected rather than expected.
     * @param int|null    $timeoutSeconds      Timeout in seconds for reading of all messages.
     * @param int|null    $timeoutMicroseconds Additional timeout in microseconds for reading of all messages.
     * @param string      $ignoreErrorFor      Ignore error for supplied string in the message.
     *
     * @return bool
     * @throws \Exception
     */
    private function expectLogEntry(
        string $type,
        string $message,
        string $pool = null,
        int $count = 1,
        bool $checkAllLogs = false,
        bool $invert = false,
        int $timeoutSeconds = null,
        int $timeoutMicroseconds = null,
        string $ignoreErrorFor = LogTool::DEBUG
    ): bool {
        for ($i = 0; $i < $count; $i++) {
            $result = $this->logTool->expectEntry(
                $type,
                $message,
                $pool,
                $ignoreErrorFor,
                $checkAllLogs,
                $invert,
                $timeoutSeconds,
                $timeoutMicroseconds,
            );

            if ( ! $result) {
                return false;
            }
        }

        return true;
    }

    /**
     * Expect a log debug message.
     *
     * @param string      $message             The expected message.
     * @param string|null $pool                The pool for pool prefixed log entry.
     * @param int         $count               The number of items.
     * @param bool        $checkAllLogs        Whether to also check past logs.
     * @param bool        $invert              Whether the log entry is not expected rather than expected.
     * @param int|null    $timeoutSeconds      Timeout in seconds for reading of all messages.
     * @param int|null    $timeoutMicroseconds Additional timeout in microseconds for reading of all messages.
     *
     * @return bool
     * @throws \Exception
     */
    public function expectLogDebug(
        string $message,
        string $pool = null,
        int $count = 1,
        bool $checkAllLogs = false,
        bool $invert = false,
        int $timeoutSeconds = null,
        int $timeoutMicroseconds = null
    ): bool {
        return $this->expectLogEntry(
            LogTool::DEBUG,
            $message,
            $pool,
            $count,
            $checkAllLogs,
            $invert,
            $timeoutSeconds,
            $timeoutMicroseconds,
            LogTool::ERROR
        );
    }

    /**
     * Expect a log notice.
     *
     * @param string      $message             The expected message.
     * @param string|null $pool                The pool for pool prefixed log entry.
     * @param int         $count               The number of items.
     * @param bool        $checkAllLogs        Whether to also check past logs.
     * @param bool        $invert              Whether the log entry is not expected rather than expected.
     * @param int|null    $timeoutSeconds      Timeout in seconds for reading of all messages.
     * @param int|null    $timeoutMicroseconds Additional timeout in microseconds for reading of all messages.
     *
     * @return bool
     * @throws \Exception
     */
    public function expectLogNotice(
        string $message,
        string $pool = null,
        int $count = 1,
        bool $checkAllLogs = false,
        bool $invert = false,
        int $timeoutSeconds = null,
        int $timeoutMicroseconds = null
    ): bool {
        return $this->expectLogEntry(
            LogTool::NOTICE,
            $message,
            $pool,
            $count,
            $checkAllLogs,
            $invert,
            $timeoutSeconds,
            $timeoutMicroseconds
        );
    }

    /**
     * Expect a log warning.
     *
     * @param string      $message             The expected message.
     * @param string|null $pool                The pool for pool prefixed log entry.
     * @param int         $count               The number of items.
     * @param bool        $checkAllLogs        Whether to also check past logs.
     * @param bool        $invert              Whether the log entry is not expected rather than expected.
     * @param int|null    $timeoutSeconds      Timeout in seconds for reading of all messages.
     * @param int|null    $timeoutMicroseconds Additional timeout in microseconds for reading of all messages.
     *
     * @return bool
     * @throws \Exception
     */
    public function expectLogWarning(
        string $message,
        string $pool = null,
        int $count = 1,
        bool $checkAllLogs = false,
        bool $invert = false,
        int $timeoutSeconds = null,
        int $timeoutMicroseconds = null
    ): bool {
        return $this->expectLogEntry(
            LogTool::WARNING,
            $message,
            $pool,
            $count,
            $checkAllLogs,
            $invert,
            $timeoutSeconds,
            $timeoutMicroseconds
        );
    }

    /**
     * Expect a log error.
     *
     * @param string      $message             The expected message.
     * @param string|null $pool                The pool for pool prefixed log entry.
     * @param int         $count               The number of items.
     * @param bool        $checkAllLogs        Whether to also check past logs.
     * @param bool        $invert              Whether the log entry is not expected rather than expected.
     * @param int|null    $timeoutSeconds      Timeout in seconds for reading of all messages.
     * @param int|null    $timeoutMicroseconds Additional timeout in microseconds for reading of all messages.
     *
     * @return bool
     * @throws \Exception
     */
    public function expectLogError(
        string $message,
        string $pool = null,
        int $count = 1,
        bool $checkAllLogs = false,
        bool $invert = false,
        int $timeoutSeconds = null,
        int $timeoutMicroseconds = null
    ): bool {
        return $this->expectLogEntry(
            LogTool::ERROR,
            $message,
            $pool,
            $count,
            $checkAllLogs,
            $invert,
            $timeoutSeconds,
            $timeoutMicroseconds
        );
    }

    /**
     * Expect a log alert.
     *
     * @param string      $message             The expected message.
     * @param string|null $pool                The pool for pool prefixed log entry.
     * @param int         $count               The number of items.
     * @param bool        $checkAllLogs        Whether to also check past logs.
     * @param bool        $invert              Whether the log entry is not expected rather than expected.
     * @param int|null    $timeoutSeconds      Timeout in seconds for reading of all messages.
     * @param int|null    $timeoutMicroseconds Additional timeout in microseconds for reading of all messages.
     *
     * @return bool
     * @throws \Exception
     */
    public function expectLogAlert(
        string $message,
        string $pool = null,
        int $count = 1,
        bool $checkAllLogs = false,
        bool $invert = false,
        int $timeoutSeconds = null,
        int $timeoutMicroseconds = null
    ): bool {
        return $this->expectLogEntry(
            LogTool::ALERT,
            $message,
            $pool,
            $count,
            $checkAllLogs,
            $invert,
            $timeoutSeconds,
            $timeoutMicroseconds
        );
    }

    /**
     * Expect no log lines to be logged.
     *
     * @return bool
     * @throws \Exception
     */
    public function expectNoLogMessages(): bool
    {
        $logLine = $this->logReader->getLine(timeoutSeconds: 0, timeoutMicroseconds: 1000);
        if ($logLine === "") {
            $logLine = $this->logReader->getLine(timeoutSeconds: 0, timeoutMicroseconds: 1000);
        }
        if ($logLine !== null) {
            return $this->error(
                "Expected no log lines but following line logged: $logLine"
            );
        }
        $this->trace('No log message received as expected');

        return true;
    }

    /**
     * Expect log config options
     *
     * @param array $options
     *
     * @return bool
     * @throws \Exception
     */
    public function expectLogConfigOptions(array $options)
    {
        foreach ($options as $value) {
            $confValue = str_replace(
                ']',
                '\]',
                str_replace(
                    '[',
                    '\[',
                    str_replace('/', '\/', $value)
                )
            );
            $this->expectLogNotice("\s+$confValue", checkAllLogs: true);
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

    /**
     * Return content of access log.
     *
     * @return string|false
     */
    public function getAccessLog()
    {
        $accessLog = $this->getFile('acc.log');
        if (is_file($accessLog)) {
            return file_get_contents($accessLog);
        }
        return false;
    }

    /**
     * Expect a single access log line.
     *
     * @param string $LogLine
     * @param bool $suppressable see expectSuppressableAccessLogEntries
     */
    public function expectAccessLog(
        string $logLine,
        bool $suppressable = false
    ) {
        if (!$suppressable || $this->expectSuppressableAccessLogEntries) {
            $this->expectedAccessLogs[] = $logLine;
        }
    }

    /**
     * Checks that all access log entries previously listed as expected by
     * calling "expectAccessLog" are in the access log.
     */
    public function checkAccessLog()
    {
        if (isset($this->expectedAccessLogs)) {
            $expectedAccessLog = implode("\n", $this->expectedAccessLogs) . "\n";
        } else {
            $this->error("Called checkAccessLog but did not previous call expectAccessLog");
        }
        if ($accessLog = $this->getAccessLog()) {
            if ($expectedAccessLog !== $accessLog) {
                $this->error(sprintf(
                    "Access log was not as expected.\nEXPECTED:\n%s\n\nACTUAL:\n%s",
                    $expectedAccessLog,
                    $accessLog
                ));
            }
        } else {
            $this->error("Called checkAccessLog but access log does not exist");
        }
    }

    /**
     * Flags whether the access log check should expect to see suppressable
     * log entries, i.e. the URL is not in access.suppress_path[] config
     *
     * @param bool
     */
    public function expectSuppressableAccessLogEntries(bool $expectSuppressableAccessLogEntries)
    {
        $this->expectSuppressableAccessLogEntries = $expectSuppressableAccessLogEntries;
    }

    /*
     * Read all log entries.
     *
     * @param string      $type    The log type
     * @param string      $message The expected message
     * @param string|null $pool    The pool for pool prefixed log entry
     *
     * @return bool
     * @throws \Exception
     */
    public function readAllLogEntries(string $type, string $message, string $pool = null): bool
    {
        return $this->logTool->readAllEntries($type, $message, $pool);
    }

    /**
     * Read all log entries.
     *
     * @param string      $message The expected message
     * @param string|null $pool    The pool for pool prefixed log entry
     *
     * @return bool
     * @throws \Exception
     */
    public function readAllLogNotices(string $message, string $pool = null): bool
    {
        return $this->readAllLogEntries(LogTool::NOTICE, $message, $pool);
    }

    /**
     * Switch the logs source.
     *
     * @param string $source The source file path or name if log is a pipe.
     *
     * @throws \Exception
     */
    public function switchLogSource(string $source)
    {
        $this->trace('Switching log descriptor to:', $source);
        $this->logReader->setFileSource($source, $this->processTemplate($source));
    }

    /**
     * Trace execution by printing supplied message only in debug mode.
     *
     * @param string            $title     Trace title to print if supplied.
     * @param string|array|null $message   Message to print.
     * @param bool              $isCommand Whether message is a command array.
     */
    private function trace(
        string $title,
        string|array $message = null,
        bool $isCommand = false,
        bool $isFile = false
    ): void {
        if ($this->debug) {
            echo "\n";
            echo ">>> $title\n";
            if (is_array($message)) {
                if ($isCommand) {
                    echo implode(' ', $message) . "\n";
                } else {
                    print_r($message);
                }
            } elseif ($message !== null) {
                if ($isFile) {
                    $this->logReader->printSeparator();
                }
                echo $message . "\n";
                if ($isFile) {
                    $this->logReader->printSeparator();
                }
            }
        }
    }
}
