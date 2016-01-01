<?php
/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Ilia Alshanetsky <ilia@php.net>                             |
   |          Preston L. Bannister <pbannister@php.net>                   |
   |          Marcus Boerger <helly@php.net>                              |
   |          Shane Caraveo <shane@php.net>                               |
   |          Derick Rethans <derick@php.net>                             |
   |          Sander Roobol <sander@php.net>                              |
   | (based on version by: Stig Bakken <ssb@php.net>)                     |
   | (based on the PHP 3 test framework by Rasmus Lerdorf)                |
   +----------------------------------------------------------------------+
 */

set_time_limit(0);
while(@ob_end_clean());
if (ob_get_level()) echo "Not all buffers were deleted.\n";
error_reporting(E_ALL);

/**********************************************************************
 * QA configuration
 */

define('PHP_QA_EMAIL', 'qa-reports@lists.php.net');
define('QA_SUBMISSION_PAGE', 'http://qa.php.net/buildtest-process.php');

/**********************************************************************
 * error messages
 */

define('PCRE_MISSING_ERROR',
'+-----------------------------------------------------------+
|                       ! ERROR !                           |
| The test-suite requires that you have pcre extension      |
| enabled. To enable this extension either compile your PHP |
| with --with-pcre-regex or if you have compiled pcre as a  |
| shared module load it via php.ini.                        |
+-----------------------------------------------------------+');
define('SAFE_MODE_WARNING',
'+-----------------------------------------------------------+
|                       ! WARNING !                         |
| You are running the test-suite with "safe_mode" ENABLED ! |
|                                                           |
| Chances are high that no test will work at all,           |
| depending on how you configured "safe_mode" !             |
+-----------------------------------------------------------+');
define('TMP_MISSING',
'+-----------------------------------------------------------+
|                       ! ERROR   !                         |
| You must create /tmp for session tests to work!           |
|                                                           |
+-----------------------------------------------------------+');
define('PROC_OPEN_MISSING',
'+-----------------------------------------------------------+
|                       ! ERROR !                           |
| The test-suite requires that proc_open() is available.    |
| Please check if you disabled it in php.ini.               |
+-----------------------------------------------------------+');
define('REQ_PHP_VERSION',
'+-----------------------------------------------------------+
|                       ! ERROR !                           |
| The test-suite must be run with PHP 5 or later.           |
| You can still test older extecutables by setting          |
| TEST_PHP_EXECUTABLE and running this script with PHP 5.   |
+-----------------------------------------------------------+');
/**********************************************************************
 * information scripts
 */
define('PHP_INFO_SCRIPT','<?php echo "
PHP_SAPI=" . PHP_SAPI . "
PHP_VERSION=" . phpversion() . "
ZEND_VERSION=" . zend_version() . "
PHP_OS=" . PHP_OS . "
INCLUDE_PATH=" . get_cfg_var("include_path") . "
INI=" . realpath(get_cfg_var("cfg_file_path")) . "
SCANNED_INI=" . (function_exists(\'php_ini_scanned_files\') ?
					str_replace("\n","", php_ini_scanned_files()) :
					"** not determined **") . "
SERVER_SOFTWARE=" . (isset($_ENV[\'SERVER_SOFTWARE\']) ? $_ENV[\'SERVER_SOFTWARE\'] : \'UNKNOWN\');
?>');

define('PHP_EXTENSIONS_SCRIPT','<?php print join(get_loaded_extensions(),":"); ?>');
define('PHP_INI_SETTINGS_SCRIPT','<?php echo serialize(ini_get_all()); ?>');

/**********************************************************************
 * various utility functions
 */

function settings2array($settings, &$ini_settings)
{
	foreach($settings as $setting) {
		if (strpos($setting, '=')!==false) {
			$setting = explode("=", $setting, 2);
			$name = trim($setting[0]);
			$value = trim($setting[1]);
			$ini_settings[$name] = $value;
		}
	}
}

function settings2params(&$ini_settings)
{
	$settings = '';
	if (count($ini_settings)) {
		foreach($ini_settings as $name => $value) {
			$value = addslashes($value);
			$settings .= " -d \"".strtolower($name)."=$value\"";
		}
	}
	return $settings;
}

function generate_diff($wanted,$output)
{
	$w = explode("\n", $wanted);
	$o = explode("\n", $output);
	$w1 = array_diff_assoc($w,$o);
	$o1 = array_diff_assoc($o,$w);
	$w2 = array();
	$o2 = array();
	foreach($w1 as $idx => $val) $w2[sprintf("%03d<",$idx)] = sprintf("%03d- ", $idx+1).$val;
	foreach($o1 as $idx => $val) $o2[sprintf("%03d>",$idx)] = sprintf("%03d+ ", $idx+1).$val;
	$diff = array_merge($w2, $o2);
	ksort($diff);
	return implode("\r\n", $diff);
}

function mkpath($path,$mode = 0777) {
	$dirs = preg_split('/[\\/]/',$path);
	$path = $dirs[0];
	for($i = 1;$i < count($dirs);$i++) {
		$path .= '/'.$dirs[$i];
		@mkdir($path,$mode);
	}
}

function copyfiles($src,$new) {
	$d = dir($src);
	while (($entry = $d->read())) {
		if (is_file("$src/$entry")) {
			copy("$src/$entry", "$new/$entry");
		}
	}
	$d->close();
}

function post_result_data($query,$data)
{
	$url = QA_SUBMISSION_PAGE.'?'.$query;
	$post = "php_test_data=" . urlencode(base64_encode(preg_replace("/[\\x00]/", "[0x0]", $data)));
	$r = new HTTPRequest($url,NULL,NULL,$post);
	return $this->response_headers['Status']=='200';
} 


function execute($command, $args=NULL, $input=NULL, $cwd=NULL, $env=NULL)
{
	$data = "";
	
	if (gettype($args)=='array') {
		$args = join($args,' ');
	}
	$commandline = "$command $args";
	$proc = proc_open($commandline, array(
				0 => array('pipe', 'r'),
				1 => array('pipe', 'w')),
				$pipes, $cwd, $env);

	if (!$proc)
		return false;

	if ($input) {
		$out = fwrite($pipes[0],$input);
		if ($out != strlen($input)) {
			return NULL;
		}
	}
	
	fclose($pipes[0]);

	while (true) {
		/* hide errors from interrupted syscalls */
		$r = $pipes;
		$w = null;
		$e = null;
		$n = @stream_select($r, $w, $e, 60);

		if ($n === 0) {
			/* timed out */
			$data .= "\n ** ERROR: process timed out **\n";
			proc_terminate($proc);
			return $data;
		} else if ($n) {
			$line = fread($pipes[1], 8192);
			if (strlen($line) == 0) {
				/* EOF */
				break;
			}
			$data .= $line;
		}
	}
	$code = proc_close($proc);
	return $data;
}

function executeCode($php, $ini_overwrites, $code, $remove_headers=true, $cwd=NULL, $env=NULL)
{
	$params = NULL;
	if ($ini_overwrites) {
		$info_params = array();
		settings2array($ini_overwrites,$info_params);
		$params = settings2params($info_params);
	}
	$out = execute($php, $params, $code, $cwd, $env);
	// kill the headers
	if ($remove_headers && preg_match("/^(.*?)\r?\n\r?\n(.*)/s", $out, $match)) {
		$out = $match[2];
	}
	return $out;
}


/**********************************************************************
 * a simple request class that lets us handle http based tests
 */

class HTTPRequest
{
    public $headers = array();
    public $timeout = 4;
    public $urlparts = NULL;
    public $url = '';
    public $userAgent = 'PHP-Test-Harness';
    public $options = array();
    public $postdata = NULL;
    public $errmsg = '';
    public $errno = 0;
    public $response;
    public $response_headers;
    public $outgoing_payload;
    public $incoming_payload = '';

    /*
    URL is the full url
    headers is assoc array of outgoing http headers
    
    options may include
    timeout
    proxy_host
    proxy_port
    proxy_user
    proxy_pass
    method (GET|POST)
    
    post data is, well, post data.  It is not processed so
        multipart stuff must be prepared before calling this
        (or add it to class)
    */
    function HTTPRequest($URL, $headers=array(), $options=array(), $postdata=NULL)
    {
        $this->urlparts = @parse_url($URL);
        $this->url = $URL;
        $this->options = $options;
        $this->headers = $headers;
        $this->postdata = &$postdata;
        $this->doRequest();
    }
    
    function doRequest()
    {
        if (!$this->_validateUrl()) return;
        
        if (isset($this->options['timeout'])) 
            $this->timeout = (int)$this->options['timeout'];
    
        $this->_sendHTTP();
    }

    function _validateUrl()
    {
        if ( ! is_array($this->urlparts) ) {
            return FALSE;
        }
        if (!isset($this->urlparts['host'])) {
            $this->urlparts['host']='127.0.0.1';
        }
        if (!isset($this->urlparts['port'])) {
            $this->urlparts['port'] = 80;
        }
        if (!isset($this->urlparts['path']) || !$this->urlparts['path'])
            $this->urlparts['path'] = '/';
        return TRUE;
    }
    
    function _parseResponse()
    {
        if (preg_match("/^(.*?)\r?\n\r?\n(.*)/s", $this->incoming_payload, $match)) {
            $this->response = $match[2];
            if (preg_match("/^HTTP\/1\.. (\d+).*/s",$match[1],$status) && !$status[1]) {
                    $this->errmsg = "HTTP Response $status[1] Not Found";
                    return FALSE;
            }
            $rh = preg_split("/[\n\r]+/",$match[1]);
            $this->response_headers = array();
            foreach ($rh as $line) {
                if (strpos($line, ':')!==false) {
                    $line = explode(":", $line, 2);
                    $this->response_headers[trim($line[0])] = trim($line[1]);
                }
            }
            $this->response_headers['Status']=$status[1];
            // if no content, return false
            if(strlen($this->response) > 0) return TRUE;
        }
        $this->errmsg = 'Invalid HTTP Response';
        return FALSE;
    }
    
    function &_getRequest()
    {
        $fullpath = $this->urlparts['path'].
                    (isset($this->urlparts['query'])?'?'.$this->urlparts['query']:'').
                    (isset($this->urlparts['fragment'])?'#'.$this->urlparts['fragment']:'');
        if (isset($this->options['proxy_host'])) {
            $fullpath = 'http://'.$this->urlparts['host'].':'.$this->urlparts['port'].$fullpath;
        }
        if (isset($this->options['proxy_user'])) {
            $headers['Proxy-Authorization'] = 'Basic ' . base64_encode($this->options['proxy_user'].":".$this->options['proxy_pass']);
        }
        $headers['User-Agent'] = $this->userAgent;
        $headers['Host'] = $this->urlparts['host'];
        $headers['Content-Length'] = strlen($this->postdata);
        $headers['Content-Type'] = 'application/x-www-form-urlencoded';
        if (isset($this->headers)) {
            $headers = array_merge($headers, $this->headers);
        }
        $headertext = '';
        foreach ($headers as $k => $v) {
            $headertext .= "$k: $v\r\n";
        }
        $method = trim($this->options['method'])?strtoupper(trim($this->options['method'])):'GET';
        $this->outgoing_payload = 
                "$method $fullpath HTTP/1.0\r\n".
                $headertext."\r\n".
                $this->postdata;
        return $this->outgoing_payload;
    }
    
    function _sendHTTP()
    {
        $this->_getRequest();
        $host = $this->urlparts['host'];
        $port = $this->urlparts['port'];
        if (isset($this->options['proxy_host'])) {
            $host = $this->options['proxy_host'];
            $port = isset($this->options['proxy_port'])?$this->options['proxy_port']:8080;
        }
        // send
        if ($this->timeout > 0) {
            $fp = fsockopen($host, $port, $this->errno, $this->errmsg, $this->timeout);
        } else {
            $fp = fsockopen($host, $port, $this->errno, $this->errmsg);
        }
        if (!$fp) {
            $this->errmsg = "Connect Error to $host:$port";
            return NULL;
        }
        if ($this->timeout > 0) {
            // some builds of php do not support this, silence
            // the warning
            @socket_set_timeout($fp, $this->timeout);
        }
        if (!fputs($fp, $this->outgoing_payload, strlen($this->outgoing_payload))) {
            $this->errmsg = "Error Sending Request Data to $host";
            return NULL;
        }
        
        while ($data = fread($fp, 32768)) {
            $this->incoming_payload .= $data;
        }

        fclose($fp);

        $this->_parseResponse();
    }

# a simple test case
#$r = new HTTPRequest('http://localhost:81/info.php/path/info');
#print_r($r->response_headers);
#print $r->response;

} // end HTTPRequest


/**********************************************************************
 * main test harness
 */

class testHarness {
	public $cwd;
	public $xargs = array(
		#arg         env var                value        default   description
		'c' => array(''                    ,'file'       ,NULL    ,'configuration file, see server-tests-config.php for example'),
		'd' => array('TEST_PATHS'          ,'paths'      ,NULL    ,'colon separate path list'),
		'e' => array('TEST_PHP_ERROR_STYLE','EMACS|MSVC' ,'EMACS' ,'editor error style'),
		'h' => array(''                    ,''           ,NULL    ,'this help'),
		'i' => array('PHPRC'               ,'path|file'  ,NULL    ,'ini file to use for tests (sets PHPRC)'),
		'l' => array('TEST_PHP_LOG_FORMAT' ,'string'     ,'LEODC' ,'any combination of CDELO'),
		'm' => array('TEST_BASE_PATH'      ,'path'       ,NULL    ,'copy tests to this path before testing'),
		'n' => array('NO_PHPTEST_SUMMARY'  ,''           ,0       ,'do not print test summary'),
		'p' => array('TEST_PHP_EXECUTABLE' ,'path'       ,NULL    ,'php executable to be tested'),
		'q' => array('NO_INTERACTION'      ,''           ,0       ,'no console interaction (ie dont contact QA)'),
		'r' => array('REPORT_EXIT_STATUS'  ,''           ,0       ,'exit with status at end of execution'),
		's' => array('TEST_PHP_SRCDIR'     ,'path'       ,NULL    ,'path to php source code'),
		't' => array('TEST_PHP_DETAILED'   ,'number'     ,0       ,'level of detail output to dump'),
		'u' => array('TEST_WEB_BASE_URL'   ,'url'        ,''      ,'base url for http testing'),
		'v' => array('TEST_CONTEXT_INFO'   ,''           ,0       ,'view text executable context info'),
		'w' => array('TEST_WEB'            ,''           ,0       ,'run tests via http'),
		'x' => array('TEST_WEB_EXT'        ,'file ext'   ,'php'   ,'http file extension to use')
		);
	
	public $conf = array();
	public $test_to_run = array();
	public $test_files = array();
	public $test_results = array();
	public $failed_tests = array();
	public $exts_to_test;
	public $exts_tested = 0;
	public $exts_skipped = 0;
	public $ignored_by_ext = 0;
	public $test_dirs = array('tests', 'pear', 'ext', 'sapi');
	public $start_time;
	public $end_time;
	public $exec_info;
	public $test_executable_iscgi = false;
	public $inisettings; // the test executables settings, used for web tests
	public $iswin32 = false;
	
	public $ddash = "=====================================================================";
	public $sdash = "---------------------------------------------------------------------";

	// Default ini settings
	public $ini_overwrites = array(
			'output_handler'=>'',
			'zlib.output_compression'=>'Off',
			'open_basedir'=>'',
			'safe_mode'=>'0',
			'disable_functions'=>'',
			'output_buffering'=>'Off',
			'error_reporting'=>'4095',
			'display_errors'=>'1',
			'log_errors'=>'0',
			'html_errors'=>'0',
			'track_errors'=>'1',
			'report_memleaks'=>'1',
			'report_zend_debug'=>'0',
			'docref_root'=>'/phpmanual/',
			'docref_ext'=>'.html',
			'error_prepend_string'=>'',
			'error_append_string'=>'',
			'auto_prepend_file'=>'',
			'auto_append_file'=>'',
			'magic_quotes_runtime'=>'0',
		);	
	public $env = array();
	public $info_params = array();

	function testHarness() {
		$this->iswin32 = substr(PHP_OS, 0, 3) == "WIN";
		$this->checkRequirements();
		$this->env = $_ENV;
		$this->removeSensitiveEnvVars();
		
		$this->initializeConfiguration();
		$this->parseArgs();
		$this->setTestPaths();
		# change to working directory
		if ($this->conf['TEST_PHP_SRCDIR']) {
			@chdir($this->conf['TEST_PHP_SRCDIR']);
		}
		$this->cwd = getcwd();

		if (!$this->conf['TEST_PHP_SRCDIR'])
			$this->conf['TEST_PHP_SRCDIR'] = $this->cwd;
		if (!$this->conf['TEST_BASE_PATH'] && $this->conf['TEST_PHP_SRCDIR'])
			$this->conf['TEST_BASE_PATH'] = $this->conf['TEST_PHP_SRCDIR'];
		if ($this->iswin32) {
			$this->conf['TEST_PHP_SRCDIR'] = str_replace('/','\\',$this->conf['TEST_PHP_SRCDIR']);
			$this->conf['TEST_BASE_PATH'] = str_replace('/','\\',$this->conf['TEST_BASE_PATH']);
		}
		
		if (!$this->conf['TEST_WEB'] && !is_executable($this->conf['TEST_PHP_EXECUTABLE'])) {
			$this->error("invalid PHP executable specified by TEST_PHP_EXECUTABLE  = " .
					$this->conf['TEST_PHP_EXECUTABLE']);
			return false;
		}
		
		$this->getInstalledExtensions();
		$this->getExecutableInfo();
		$this->getExecutableIniSettings();
		$this->test_executable_iscgi = strncmp($this->exec_info['PHP_SAPI'],'cgi',3)==0;
		$this->calculateDocumentRoot();

		// add TEST_PHP_SRCDIR to the include path, this facilitates
		// tests including files from src/tests
		//$this->ini_overwrites['include_path'] = $this->cwd.($this->iswin32?';.;':':.:').$this->exec_info['INCLUDE_PATH'];
		
		$params = array();
		settings2array($this->ini_overwrites,$params);
		$this->info_params = settings2params($params);
		
		$this->contextHeader();
		if ($this->conf['TEST_CONTEXT_INFO']) return;
		$this->loadFileList();
		$this->moveTestFiles();
		$this->run();
		$this->summarizeResults();
	}

	function getExecutableIniSettings()
	{
		$out = $this->runscript(PHP_INI_SETTINGS_SCRIPT,true);
		$this->inisettings = unserialize($out);
	}
	
	function getExecutableInfo()
	{
		$out = $this->runscript(PHP_INFO_SCRIPT,true);
		$out = preg_split("/[\n\r]+/",$out);
		$info = array();
		foreach ($out as $line) {
			if (strpos($line, '=')!==false) {
				$line = explode("=", $line, 2);
				$name = trim($line[0]);
				$value = trim($line[1]);
				$info[$name] = $value;
			}
		}
		$this->exec_info = $info;
	}
	
	function getInstalledExtensions()
	{
		// get the list of installed extensions
		$out = $this->runscript(PHP_EXTENSIONS_SCRIPT,true);
		$this->exts_to_test = explode(":",$out);
		sort($this->exts_to_test);
		$this->exts_tested = count($this->exts_to_test);
	}

	// if running local, calls executeCode,
	// otherwise does an http request
	function runscript($script,$removeheaders=false,$cwd=NULL)
	{
		if ($this->conf['TEST_WEB']) {
			$pi = '/testscript.' . $this->conf['TEST_WEB_EXT'];
			if (!$cwd) $cwd = $this->conf['TEST_BASE_PATH'];
			$tmp_file = "$cwd$pi";
			$pi = substr($cwd,strlen($this->conf['TEST_BASE_PATH'])) . $pi;
			$url = $this->conf['TEST_WEB_BASE_URL'] . $pi;
			file_put_contents($tmp_file,$script);
			$fd = fopen($url, "rb");
			$out = '';
			if ($fd) {
				while (!feof($fd))
					$out .= fread($fd, 8192);
				fclose($fd);
			}
			unlink($tmp_file);
			if (0 && $removeheaders &&
				preg_match("/^(.*?)\r?\n\r?\n(.*)/s", $out, $match)) {
					return $match[2];
			}
			return $out;
		} else {
			return executeCode($this->conf['TEST_PHP_EXECUTABLE'],$this->ini_overwrites, $script,$removeheaders,$cwd,$this->env);
		}
	}

	
	// Use this function to do any displaying of text, so that
	// things can be over-written as necessary.
	
	function writemsg($msg) {
	    
	    echo $msg;
	    
	}
	
	// Another wrapper function, this one should be used any time
	// a particular test passes or fails
	
	function showstatus($item, $status, $reason = '') {
	    
	    switch($status) {
		case 'PASSED':
		    $this->writemsg("PASSED: $item ($reason)\n");
		    break;
		case 'FAILED':
		    $this->writemsg("FAILED: $item ($reason)\n");
		    break;
		case 'SKIPPED':
		    $this->writemsg("SKIPPED: $item ($reason)\n");
		    break;
	    }
	}
	
	
	function help()
	{
		$usage = "usage: php run-tests.php [options]\n";
		foreach ($this->xargs as $arg=>$arg_info) {
			$usage .= sprintf(" -%s  %-12s %s\n",$arg,$arg_info[1],$arg_info[3]);
		}
		return $usage;
	}
	
	function parseArgs() {
		global $argc;
		global $argv;
		global $_SERVER;
		
		if (!isset($argv)) {
			$argv = $_SERVER['argv'];
			$argc = $_SERVER['argc'];
		}
	
		$conf = NULL;
		for ($i=1; $i<$argc;) {
			if ($argv[$i][0] != '-') continue;
			$opt = $argv[$i++][1];
			if (isset($value)) unset($value);
			if (@$argv[$i][0] != '-') {
				@$value = $argv[$i++];
			}
			switch($opt) {
			case 'c':
				/* TODO: Implement configuraiton file */
				include($value);
				if (!isset($conf)) {
					$this->writemsg("Invalid configuration file\n");
					exit(1);
				}
				$this->conf = array_merge($this->conf,$conf);
				break;
			case 'e':
				$this->conf['TEST_PHP_ERROR_STYLE'] = strtoupper($value);
				break;
			case 'h':
				print $this->help();
				exit(0);
			default:
				if ($this->xargs[$opt][1] && isset($value))
					$this->conf[$this->xargs[$opt][0]] = $value;
				else if (!$this->xargs[$opt][1])
					$this->conf[$this->xargs[$opt][0]] = isset($value)?$value:1;
				else
					$this->error("Invalid argument setting for argument $opt, should be [{$this->xargs[$opt][1]}]\n");
				break;
			}
		}
		
		// set config into environment, this allows
		// executed tests to find out about the test
		// configurations.  config file or args overwrite
		// env var config settings
		$this->env = array_merge($this->env,$this->conf);
		if (!$this->conf['TEST_WEB'] && !$this->conf['TEST_PHP_EXECUTABLE']) {
			$this->writemsg($this->help());
			exit(0);
		}
	}

	function removeSensitiveEnvVars()
	{
		# delete sensitive env vars
		$this->env['SSH_CLIENT']='deleted';
		$this->env['SSH_AUTH_SOCK']='deleted';
		$this->env['SSH_TTY']='deleted';
	}
	
	function setEnvConfigVar($name)
	{
		if (isset($this->env[$name])) {
			$this->conf[$name] = $this->env[$name];
		}
	}
	
	function initializeConfiguration()
	{
		foreach ($this->xargs as $arg=>$arg_info) {
			if ($arg_info[0]) {
				# initialize the default setting
				$this->conf[$arg_info[0]]=$arg_info[2];
				# get config from environment
				$this->setEnvConfigVar($arg_info[0]);
			}
		}
	}

	function setTestPaths()
	{
		// configure test paths from config file or command line
		if (@$this->conf['TEST_PATHS']) {
			$this->test_dirs = array();
			if ($this->iswin32) {
				$paths = explode(';',$this->conf['TEST_PATHS']);
			} else {
				$paths = explode(':|;',$this->conf['TEST_PATHS']);
			}
			foreach($paths as $path) {
				$this->test_dirs[] = realpath($path);
			}
		}
	}
	
	function test_sort($a, $b) {
		$ta = strpos($a, "{$this->cwd}/tests")===0 ? 1 + (strpos($a, "{$this->cwd}/tests/run-test")===0 ? 1 : 0) : 0;
		$tb = strpos($b, "{$this->cwd}/tests")===0 ? 1 + (strpos($b, "{$this->cwd}/tests/run-test")===0 ? 1 : 0) : 0;
		if ($ta == $tb) {
			return strcmp($a, $b);
		} else {
			return $tb - $ta;
		}
	}

	function checkRequirements() {
		if (version_compare(phpversion(), "5.0") < 0) {
			$this->writemsg(REQ_PHP_VERSION);
			exit;
		}
// We might want to check another server so we won't see that server's /tmp
//		if (!file_exists("/tmp")) {
//			$this->writemsg(TMP_MISSING);
//			exit;
//		}
		if (!function_exists("proc_open")) {
			$this->writemsg(PROC_OPEN_MISSING);
			exit;
		}
		if (!extension_loaded("pcre")) {
			$this->writemsg(PCRE_MISSING_ERROR);
			exit;
		}
		if (ini_get('safe_mode')) {
			$this->writemsg(SAFE_MODE_WARNING);
		}
	}
	
	//
	// Write test context information.
	//
	function contextHeader()
	{
		$info = '';
		foreach ($this->exec_info as $k=>$v) {
			$info .= sprintf("%-20.s: %s\n",$k,$v);
		}
		$exts = '';
		foreach ($this->exts_to_test as $ext) {
			$exts .="$ext\n              ";
		}
		$dirs = '';
		foreach ($this->test_dirs as $test_dir) {
			$dirs .= "$test_dir\n              ";
		}
		$conf = '';
		foreach ($this->conf as $k=>$v) {
			$conf .= sprintf("%-20.s: %s\n",$k,$v);
		}
		
		$exeinfo = '';
		if (!$this->conf['TEST_WEB'])
			$exeinfo = "CWD                 : {$this->cwd}\n".
					"PHP                 : {$this->conf['TEST_PHP_EXECUTABLE']}\n";
		
		$this->writemsg("\n$this->ddash\n".
			"$exeinfo$info\n".
			"Test Harness Configuration:\n$conf\n".
			"Extensions  : $exts\n".
			"Test Dirs   : $dirs\n".
			"$this->ddash\n");
	}
	
	function loadFileList()
	{
		foreach ($this->test_dirs as $dir) {
			if (is_dir($dir)) {
				$this->findFilesInDir($dir, ($dir == 'ext'));
			} else {
				$this->test_files[] = $dir;
			}
		}
		usort($this->test_files,array($this,"test_sort"));
		$this->writemsg("found ".count($this->test_files)." files\n");
	}
	
	function moveTestFiles()
	{
		if (!$this->conf['TEST_BASE_PATH'] ||
			$this->conf['TEST_BASE_PATH'] == $this->conf['TEST_PHP_SRCDIR']) return;
		$this->writemsg("moving files from {$this->conf['TEST_PHP_SRCDIR']} to {$this->conf['TEST_BASE_PATH']}\n");
		$l = strlen($this->conf['TEST_PHP_SRCDIR']);
		$files = array();
		$dirs = array();
		foreach ($this->test_files as $file) {
			if (strpos($file,$this->conf['TEST_PHP_SRCDIR'])==0) {
				$newlocation = $this->conf['TEST_BASE_PATH'].substr($file,$l);
				$files[] = $newlocation;
				$dirs[dirname($file)] = dirname($newlocation);
			} else {
				// XXX what to do with test files outside the
				// php source directory?  Need to map them into
				// the new directory somehow.
			}
		}
		foreach ($dirs as $src=>$new) {
			mkpath($new);
			copyfiles($src,$new);
		}
		$this->test_files = $files;
	}
	
	function findFilesInDir($dir,$is_ext_dir=FALSE,$ignore=FALSE)
	{
		$skip = array('.', '..', 'CVS');
		$o = opendir($dir) or $this->error("cannot open directory: $dir");
		while (($name = readdir($o)) !== FALSE) {
			if (in_array($name, $skip)) continue;
			if (is_dir("$dir/$name")) {
				$skip_ext = ($is_ext_dir && !in_array($name, $this->exts_to_test));
				if ($skip_ext) {
					$this->exts_skipped++;
				}
				$this->findFilesInDir("$dir/$name", FALSE, $ignore || $skip_ext);
			}
	
			// Cleanup any left-over tmp files from last run.
			if (substr($name, -4) == '.tmp') {
				@unlink("$dir/$name");
				continue;
			}
	
			// Otherwise we're only interested in *.phpt files.
			if (substr($name, -5) == '.phpt') {
				if ($ignore) {
					$this->ignored_by_ext++;
				} else {
					$testfile = realpath("$dir/$name");
					$this->test_files[] = $testfile;
				}
			}
		}
		closedir($o);
	}
	
	function runHeader()
	{
		$this->writemsg("TIME START " . date('Y-m-d H:i:s', $this->start_time) . "\n".$this->ddash."\n");
		if (count($this->test_to_run)) {
			$this->writemsg("Running selected tests.\n");
		} else {
			$this->writemsg("Running all test files.\n");
		}
	}
	
	function run()
	{
		$this->start_time = time();
		$this->runHeader();
		// Run selected tests.
		if (count($this->test_to_run)) {
			
			foreach($this->test_to_run as $name=>$runnable) {
				if(!preg_match("/\.phpt$/", $name))
					continue;
				if ($runnable) {
					$this->test_results[$name] = $this->run_test($name);
				}
			}
		} else {
			foreach ($this->test_files as $name) {
				$this->test_results[$name] = $this->run_test($name);
			}
		}
		$this->end_time = time();
	}

	function summarizeResults()
	{
		if (count($this->test_results) == 0) {
			$this->writemsg("No tests were run.\n");
			return;
		}
		
		$n_total = count($this->test_results);
		$n_total += $this->ignored_by_ext;
		
		$sum_results = array('PASSED'=>0, 'SKIPPED'=>0, 'FAILED'=>0);
		foreach ($this->test_results as $v) {
			$sum_results[$v]++;
		}
		$sum_results['SKIPPED'] += $this->ignored_by_ext;
		$percent_results = array();
		while (list($v,$n) = each($sum_results)) {
			$percent_results[$v] = (100.0 * $n) / $n_total;
		}
		
		$this->writemsg("\n".$this->ddash."\n".
			"TIME END " . date('Y-m-d H:i:s', $this->end_time) . "\n".
			$this->ddash."\n".
			"TEST RESULT SUMMARY\n".
			$this->sdash."\n".
			"Exts skipped    : " . sprintf("%4d",$this->exts_skipped) . "\n".
			"Exts tested     : " . sprintf("%4d",$this->exts_tested) . "\n".
			$this->sdash."\n".
			"Number of tests : " . sprintf("%4d",$n_total) . "\n".
			"Tests skipped   : " . sprintf("%4d (%2.1f%%)",$sum_results['SKIPPED'],$percent_results['SKIPPED']) . "\n".
			"Tests failed    : " . sprintf("%4d (%2.1f%%)",$sum_results['FAILED'],$percent_results['FAILED']) . "\n".
			"Tests passed    : " . sprintf("%4d (%2.1f%%)",$sum_results['PASSED'],$percent_results['PASSED']) . "\n".
			$this->sdash."\n".
			"Time taken      : " . sprintf("%4d seconds", $this->end_time - $this->start_time) . "\n".
			$this->ddash."\n");
		
		$failed_test_summary = '';
		if ($this->failed_tests) {
			$failed_test_summary .= "\n".$this->ddash."\n".
				"FAILED TEST SUMMARY\n".$this->sdash."\n";
			foreach ($this->failed_tests as $failed_test_data) {
				$failed_test_summary .=  $failed_test_data['test_name'] . "\n";
			}
			$failed_test_summary .=  $this->ddash."\n";
		}
		
		if ($failed_test_summary && !$this->conf['NO_PHPTEST_SUMMARY']) {
			$this->writemsg($failed_test_summary);
		}

		/* We got failed Tests, offer the user to send and e-mail to QA team, unless NO_INTERACTION is set */
		if ($sum_results['FAILED'] && !$this->conf['NO_INTERACTION']) {
			$fp = fopen("php://stdin", "r+");
			$this->writemsg("\nPlease allow this report to be send to the PHP QA\nteam. This will give us a better understanding in how\n");
			$this->writemsg("PHP's test cases are doing.\n");
			$this->writemsg("(choose \"s\" to just save the results to a file)? [Yns]: ");
			flush();
			$user_input = fgets($fp, 10);
			$just_save_results = (strtolower($user_input[0]) == 's');
			
			if ($just_save_results || strlen(trim($user_input)) == 0 || strtolower($user_input[0]) == 'y') {
				/*  
				 * Collect information about the host system for our report
				 * Fetch phpinfo() output so that we can see the PHP environment
				 * Make an archive of all the failed tests
				 * Send an email
				 */

				/* Ask the user to provide an email address, so that QA team can contact the user */
				if (!strncasecmp($user_input, 'y', 1) || strlen(trim($user_input)) == 0) {
					echo "\nPlease enter your email address.\n(Your address will be mangled so that it will not go out on any\nmailinglist in plain text): ";
					flush();
					$fp = fopen("php://stdin", "r+");
					$user_email = trim(fgets($fp, 1024));
					$user_email = str_replace("@", " at ", str_replace(".", " dot ", $user_email));
				}
		
				$failed_tests_data = '';
				$sep = "\n" . str_repeat('=', 80) . "\n";
				
				$failed_tests_data .= $failed_test_summary . "\n";
				
				if (array_sum($this->failed_tests)) {
					foreach ($this->failed_tests as $test_info) {
						$failed_tests_data .= $sep . $test_info['name'];
						$failed_tests_data .= $sep . file_get_contents(realpath($test_info['output']));
						$failed_tests_data .= $sep . file_get_contents(realpath($test_info['diff']));
						$failed_tests_data .= $sep . "\n\n";
					}
					$status = "failed";
				} else {
					$status = "success";
				}
				
				$failed_tests_data .= "\n" . $sep . 'BUILD ENVIRONMENT' . $sep;
				$failed_tests_data .= "OS:\n". PHP_OS. "\n\n";
				$automake = $autoconf = $libtool = $compiler = 'N/A';

				if (!$this->iswin32) {
					$automake = shell_exec('automake --version');
					$autoconf = shell_exec('autoconf --version');
					/* Always use the generated libtool - Mac OSX uses 'glibtool' */
					$libtool = shell_exec('./libtool --version');
					/* Try the most common flags for 'version' */
					$flags = array('-v', '-V', '--version');
					$cc_status=0;
					foreach($flags AS $flag) {
						system($this->env['CC']." $flag >/dev/null 2>&1", $cc_status);
						if($cc_status == 0) {
							$compiler = shell_exec($this->env['CC']." $flag 2>&1");
							break;
						}
					}
				}
				
				$failed_tests_data .= "Automake:\n$automake\n";
				$failed_tests_data .= "Autoconf:\n$autoconf\n";
				$failed_tests_data .= "Libtool:\n$libtool\n";
				$failed_tests_data .= "Compiler:\n$compiler\n";
				$failed_tests_data .= "Bison:\n". @shell_exec('bison --version'). "\n";
				$failed_tests_data .= "\n\n";

				if (isset($user_email)) {
					$failed_tests_data .= "User's E-mail: ".$user_email."\n\n";
				}

				$failed_tests_data .= $sep . "PHPINFO" . $sep;
				$failed_tests_data .= shell_exec($this->conf['TEST_PHP_EXECUTABLE'].' -dhtml_errors=0 -i');
				
				$compression = 0;

				if ($just_save_results ||
					!post_result_data("status=$status&version=".urlencode(TESTED_PHP_VERSION),$failed_tests_data)) {
					$output_file = 'php_test_results_' . date('Ymd_Hi') . ( $compression ? '.txt.gz' : '.txt' );
					$fp = fopen($output_file, "w");
					fwrite($fp, $failed_tests_data);
					fclose($fp);
				
					if (!$just_save_results)
						echo "\nThe test script was unable to automatically send the report to PHP's QA Team\n";
					echo "Please send ".$output_file." to ".PHP_QA_EMAIL." manually, thank you.\n";
				} else {
					fwrite($fp, "\nThank you for helping to make PHP better.\n");
					fclose($fp);
				}
			}
		}
		 
		if($this->conf['REPORT_EXIT_STATUS'] and $sum_results['FAILED']) {
			exit(1);
		}
	}

	function getINISettings(&$section_text)
	{
		$ini_settings = $this->ini_overwrites;
		// Any special ini settings 
		// these may overwrite the test defaults...
		if (array_key_exists('INI', $section_text)) {
			settings2array(preg_split( "/[\n\r]+/", $section_text['INI']), $ini_settings);
		}
		return $ini_settings;
	}

	function getINIParams(&$section_text)
	{
		if (!$section_text) return '';
		// XXX php5 current has a problem doing this in one line
		// it fails with Only variables can be passed by reference
		// on test ext\calendar\tests\jdtojewish.phpt
		// return settings2params($this->getINISettings($section_text));
		$ini = $this->getINISettings($section_text);
		return settings2params($ini);
	}

	function calculateDocumentRoot()
	{
		if ($this->conf['TEST_WEB'] || $this->test_executable_iscgi) {
			// configure DOCUMENT_ROOT for web tests
			// this assumes that directories from the base url
			// matches directory depth from the base path
			$parts = parse_url($this->conf['TEST_WEB_BASE_URL']);
			$depth = substr_count($parts['path'],'/');
			$docroot = $this->conf['TEST_BASE_PATH'];
			for ($i=0 ; $i < $depth; $i++) $docroot = dirname($docroot);
			$this->conf['TEST_DOCUMENT_ROOT']=$docroot;
			$this->conf['TEST_BASE_SCRIPT_NAME']=$parts['path'];
			$this->conf['TEST_SERVER_URL']=substr($this->conf['TEST_WEB_BASE_URL'],0,strlen($this->conf['TEST_WEB_BASE_URL'])-strlen($parts['path']));
		} else {
			$this->conf['TEST_DOCUMENT_ROOT']='';
			$this->conf['TEST_BASE_SCRIPT_NAME']='';
			$this->conf['TEST_SERVER_URL']='';
		}
	}

	function evalSettings($filename,$data) {
		// we eval the section so we can allow dynamic env vars
		// for cgi testing
		$filename = str_replace('\\','/',$filename);
		$cwd = str_replace('\\','/',$this->cwd);
		$filepath = dirname($filename);
		$scriptname = substr($filename,strlen($this->conf['TEST_DOCUMENT_ROOT']));
		// eval fails if no newline
		return eval("$data\n");
	}
	
	function getENVSettings(&$section_text,$testfile)
	{
		$env = $this->env;
		// Any special environment settings 
		// these may overwrite the test defaults...
		if (array_key_exists('ENV', $section_text)) {
			$sect = $this->evalSettings($testfile,$section_text['ENV']);
			//print "data evaled:\n$sect\n";
			settings2array(preg_split( "/[\n\r]+/", $sect), $env);
		}
		return $env;
	}

	function getEvalTestSettings($section_text,$testfile)
	{
		$rq = array();
		// Any special environment settings 
		// these may overwrite the test defaults...
		if ($section_text) {
			$sect = $this->evalSettings($testfile,$section_text);
			//print "data evaled:\n$sect\n";
			settings2array(preg_split( "/[\n\r]+/", $sect), $rq);
		}
		return $rq;
	}
	
	//
	// Load the sections of the test file.
	//
	function getSectionText($file)
	{
		// Load the sections of the test file.
		$section_text = array(
			'TEST'   => '(unnamed test)',
			'SKIPIF' => '',
			'GET'    => '',
			'ARGS'   => '',
			'_FILE'   => $file,
			'_DIR'    => realpath(dirname($file)),
		);
	
		$fp = @fopen($file, "r")
				or $this->error("Cannot open test file: $file");
	
		$section = '';
		while (!feof($fp)) {
			$line = fgets($fp);
			// Match the beginning of a section.
			if (preg_match('/^--([A-Z]+)--/',$line,$r)) {
				$section = $r[1];
				$section_text[$section] = '';
				continue;
			}
			
			// Add to the section text.
			$section_text[$section] .= $line;
		}
		fclose($fp);
		foreach ($section_text as $k=>$v) {
			// for POST data ,we only want to trim the last new line!
			if ($k == 'POST' && preg_match('/^(.*?)\r?\n$/Ds',$v,$matches)) {
				$section_text[$k]=$matches[1];
			} else {
				$section_text[$k]=trim($v);
			}
		}
		return $section_text;
	}

	//
	// Check if test should be skipped.
	//
	function getSkipReason($file,&$section_text,$docgi=false)
	{
		// if the test uses POST or GET, and it's not the cgi
		// executable, skip
		if ($docgi && !$this->conf['TEST_WEB'] && !$this->test_executable_iscgi) {
			$this->showstatus($section_text['TEST'], 'SKIPPED', 'CGI Test needs CGI Binary');
			return "SKIPPED";
		}
		// if we're doing web testing, then we wont be able to set
		// ini setting on the command line.  be sure the executables
		// ini settings are compatible with the test, or skip
		if (($docgi || $this->conf['TEST_WEB']) &&
			isset($section_text['INI']) && $section_text['INI']) {
			$settings = $this->getINISettings($section_text);
			foreach ($settings as $k=>$v) {
				if (strcasecmp($v,'off')==0 || !$v) $v='';
				$haveval = isset($this->inisettings[$k]['local_value']);
				if ($k == 'include_path') {
					// we only want to know that src directory
					// is in the include path
					if (strpos($this->inisettings[$k]['local_value'],$this->cwd))
						continue;
				}
				if (($haveval && $this->inisettings[$k]['local_value'] != $v) || (!$haveval && $v)) {
					$this->showstatus($section_text['TEST'], 'SKIPPED', "Test requires ini setting $k=[$v], not [".($haveval?$this->inisettings[$k]['local_value']:'')."]");
					return "SKIPPED";
				}
			}
		}
		// now handle a SKIPIF section
		if ($section_text['SKIPIF']) {
			$output = trim($this->runscript($section_text['SKIPIF'],$this->test_executable_iscgi,realpath(dirname($file))),true);
			if (!$output) return NULL;
			if ($this->conf['TEST_PHP_DETAILED'] > 2)
				print "SKIPIF: [$output]\n";
			if (preg_match("/^skip/i", $output)){
			
				$reason = (preg_match("/^skip\s*(.+)\$/", $output)) ? preg_replace("/^skip\s*(.+)\$/", "\\1", $output) : FALSE;
				$this->showstatus($section_text['TEST'], 'SKIPPED', $reason);
				return 'SKIPPED';
			}
			if (preg_match("/^info/i", $output)) {
				$reason = (preg_match("/^info\s*(.+)\$/", $output)) ? preg_replace("/^info\s*(.+)\$/", "\\1", $output) : FALSE;
				if ($reason) {
					$tested .= " (info: $reason)";
				}
			}
		}
		return NULL;
	}

	//
	//  Run an individual test case.
	//
	function run_test($file)
	{
		if ($this->conf['TEST_PHP_DETAILED'])
			$this->writemsg("\n=================\nTEST $file\n");
	
		$section_text = $this->getSectionText($file);
	
		if ($this->iswin32)
			$shortname = str_replace($this->conf['TEST_BASE_PATH'].'\\', '', $file);
		else
			$shortname = str_replace($this->conf['TEST_BASE_PATH'].'/', '', $file);
		$tested = $section_text['TEST']." [$shortname]";
	
		if ($this->conf['TEST_WEB']) {
			$tmp_file   = preg_replace('/\.phpt$/','.'.$this->conf['TEST_WEB_EXT'],$file);
			$uri = $this->conf['TEST_BASE_SCRIPT_NAME'].str_replace($this->conf['TEST_BASE_PATH'], '', $tmp_file);
			$uri = str_replace('\\', '/', $uri);
		} else {
			$tmp_file   = preg_replace('/\.phpt$/','.php',$file);
		}
		@unlink($tmp_file);
	
		// unlink old test results	
		@unlink(preg_replace('/\.phpt$/','.diff',$file));
		@unlink(preg_replace('/\.phpt$/','.log',$file));
		@unlink(preg_replace('/\.phpt$/','.exp',$file));
		@unlink(preg_replace('/\.phpt$/','.out',$file));
	
		if (!$this->conf['TEST_WEB']) {
			// Reset environment from any previous test.
			$env = $this->getENVSettings($section_text,$tmp_file);
			$ini_overwrites = $this->getINIParams($section_text);
		}
		
		// if this is a cgi test, prepare for it
		$query_string = '';
		$havepost = array_key_exists('POST', $section_text) && !empty($section_text['POST']);
		// allow empty query_string requests
		$haveget = array_key_exists('GET', $section_text) && !empty($section_text['GET']);
		$do_cgi = array_key_exists('CGI', $section_text) || $haveget || $havepost;

		$skipreason = $this->getSkipReason($file,$section_text,$do_cgi);
		if ($skipreason == 'SKIPPED') {
			return $skipreason;
		}

		// We've satisfied the preconditions - run the test!
		file_put_contents($tmp_file,$section_text['FILE']);

		$post = NULL;
		$args = "";

		$headers = array();
		if ($this->conf['TEST_WEB']) {
			$request = $this->getEvalTestSettings(@$section_text['REQUEST'],$tmp_file);
			$headers = $this->getEvalTestSettings(@$section_text['HEADERS'],$tmp_file);

			$method = isset($request['method'])?$request['method']:$havepost?'POST':'GET';
			$query_string = $haveget?$section_text['GET']:'';
		
			$options = array();
			$options['method']=$method;
			if (isset($this->conf['timeout']))    $options['timeout']    = $this->conf['timeout'];
			if (isset($this->conf['proxy_host'])) $options['proxy_host'] = $this->conf['proxy_host'];
			if (isset($this->conf['proxy_port'])) $options['proxy_port'] = $this->conf['proxy_port'];
			if (isset($this->conf['proxy_user'])) $options['proxy_user'] = $this->conf['proxy_user'];
			if (isset($this->conf['proxy_pass'])) $options['proxy_pass'] = $this->conf['proxy_pass'];
			
			$post = $havepost?$section_text['POST']:NULL;
			$url = $this->conf['TEST_SERVER_URL'];
			if (isset($request['SCRIPT_NAME']))
				$url .= $request['SCRIPT_NAME'];
			else
				$url .= $uri;
			if (isset($request['PATH_INFO']))
				$url .= $request['PATH_INFO'];
			if (isset($request['FRAGMENT']))
				$url .= '#'.$request['FRAGMENT'];
			if (isset($request['QUERY_STRING']))
				$query_string = $request['QUERY_STRING'];
			if ($query_string)
				$url .= '?'.$query_string;
			if ($this->conf['TEST_PHP_DETAILED'])
				$this->writemsg("\nURL  = $url\n");
		} else if ($do_cgi) {
			$query_string = $haveget?$section_text['GET']:'';
			
			if (!array_key_exists('GATEWAY_INTERFACE', $env))
				$env['GATEWAY_INTERFACE']='CGI/1.1';
			if (!array_key_exists('SERVER_SOFTWARE', $env))
				$env['SERVER_SOFTWARE']='PHP Test Harness';
			if (!array_key_exists('SERVER_SOFTWARE', $env))
				$env['SERVER_NAME']='127.0.0.1';
			if (!array_key_exists('REDIRECT_STATUS', $env))
				$env['REDIRECT_STATUS']='200';
			if (!array_key_exists('SERVER_NAME', $env))
				$env['QUERY_STRING']=$query_string;
			if (!array_key_exists('PATH_TRANSLATED', $env) &&
				!array_key_exists('SCRIPT_FILENAME', $env)) {
				$env['PATH_TRANSLATED']=$tmp_file;
				$env['SCRIPT_FILENAME']=$tmp_file;
			}
			if (!array_key_exists('PATH_TRANSLATED', $env))
				$env['PATH_TRANSLATED']='';
			if (!array_key_exists('PATH_INFO', $env))
				$env['PATH_INFO']='';
			if (!array_key_exists('SCRIPT_NAME', $env))
				$env['SCRIPT_NAME']='';
			if (!array_key_exists('SCRIPT_FILENAME', $env))
				$env['SCRIPT_FILENAME']='';
		
			if (array_key_exists('POST', $section_text) && (!$haveget || !empty($section_text['POST']))) {
				$post = $section_text['POST'];
				$content_length = strlen($post);
				if (!array_key_exists('REQUEST_METHOD', $env))
					$env['REQUEST_METHOD']='POST';
				if (!array_key_exists('CONTENT_TYPE', $env))
					$env['CONTENT_TYPE']='application/x-www-form-urlencoded';
				if (!array_key_exists('CONTENT_LENGTH', $env))
					$env['CONTENT_LENGTH']=$content_length;
			} else {
				if (!array_key_exists('REQUEST_METHOD', $env))
					$env['REQUEST_METHOD']='GET';
				if (!array_key_exists('CONTENT_TYPE', $env))
					$env['CONTENT_TYPE']='';
				if (!array_key_exists('CONTENT_LENGTH', $env))
					$env['CONTENT_LENGTH']='';
			}
			if ($this->conf['TEST_PHP_DETAILED'] > 1)
				$this->writemsg("\nCONTENT_LENGTH  = " . $env['CONTENT_LENGTH'] . 
						"\nCONTENT_TYPE    = " . $env['CONTENT_TYPE'] . 
						"\nPATH_TRANSLATED = " . $env['PATH_TRANSLATED'] . 
						"\nPATH_INFO       = " . $env['PATH_INFO'] . 
						"\nQUERY_STRING    = " . $env['QUERY_STRING'] . 
						"\nREDIRECT_STATUS = " . $env['REDIRECT_STATUS'] . 
						"\nREQUEST_METHOD  = " . $env['REQUEST_METHOD'] . 
						"\nSCRIPT_NAME     = " . $env['SCRIPT_NAME'] . 
						"\nSCRIPT_FILENAME = " . $env['SCRIPT_FILENAME'] . "\n");
			/* not cgi spec to put query string on command line,
			   but used by a couple tests to catch a security hole
			   in older php versions.  At least IIS can be configured
			   to do this. */
			$args = $env['QUERY_STRING'];
			$args = "$ini_overwrites $tmp_file \"$args\" 2>&1";
		} else {
			$args = $section_text['ARGS'] ? $section_text['ARGS'] : '';
			$args = "$ini_overwrites $tmp_file $args 2>&1";
		}

		if ($this->conf['TEST_WEB']) {
			// we want headers also, so fopen
			$r = new HTTPRequest($url,$headers,$options,$post);
			//$out = preg_replace("/\r\n/","\n",$r->response);
			$out = $r->response;
			$headers = $r->response_headers;
			//print $r->outgoing_payload."\n";
			//print $r->incoming_payload."\n";
		} else {
			$out = execute($this->conf['TEST_PHP_EXECUTABLE'],$args,$post,$this->cwd,$env);
			// if this is a cgi, remove the headers first
			if ($this->test_executable_iscgi
				 && preg_match("/^(.*?)\r?\n\r?\n(.*)/s", $out, $match)) {
				$out = $match[2];
				$rh = preg_split("/[\n\r]+/",$match[1]);
				$headers = array();
				foreach ($rh as $line) {
					if (strpos($line, ':')!==false) {
						$line = explode(":", $line, 2);
						$headers[trim($line[0])] = trim($line[1]);
					}
				}
			}
		}
		
		if ($this->conf['TEST_PHP_DETAILED'] > 2) {
			echo "HEADERS: ";
			print_r($headers);
			echo "OUTPUT: \n$out\n";
			
		}
			
		// Does the output match what is expected?
		$output = trim($out);
		$output = preg_replace('/\r\n/',"\n",$output);

		$failed = FALSE;

		if (isset($section_text['EXPECTF']) || isset($section_text['EXPECTREGEX'])) {
			if (isset($section_text['EXPECTF'])) {
				$wanted = $section_text['EXPECTF'];
			} else {
				$wanted = $section_text['EXPECTREGEX'];
			}
			$wanted_re = preg_replace('/\r\n/',"\n",$wanted);
			if (isset($section_text['EXPECTF'])) {
				// do preg_quote, but miss out any %r delimited sections
				$temp = "";
				$r = "%r";
				$startOffset = 0;
				$length = strlen($wanted_re);
				while($startOffset < $length) {
					$start = strpos($wanted_re, $r, $startOffset);
					if ($start !== false) {
						// we have found a start tag
						$end = strpos($wanted_re, $r, $start+2);
						if ($end === false) {
							// unbalanced tag, ignore it.
							$end = $start = $length;
						}
					} else {
						// no more %r sections
						$start = $end = $length;
					}
					// quote a non re portion of the string
					$temp = $temp . preg_quote(substr($wanted_re, $startOffset, ($start - $startOffset)),  '/');
					// add the re unquoted.
					if ($end > $start) {
						$temp = $temp . '(' . substr($wanted_re, $start+2, ($end - $start-2)). ')';
					}
					$startOffset = $end + 2;
				}
				$wanted_re = $temp;

				$wanted_re = str_replace(
					array('%binary_string_optional%'),
					'string',
					$wanted_re
				);
				$wanted_re = str_replace(
					array('%unicode_string_optional%'),
					'string',
					$wanted_re
				);
				$wanted_re = str_replace(
					array('%unicode\|string%', '%string\|unicode%'),
					'string',
					$wanted_re
				);
				$wanted_re = str_replace(
					array('%u\|b%', '%b\|u%'),
					'',
					$wanted_re
				);
				// Stick to basics
				$wanted_re = str_replace('%e', '\\' . DIRECTORY_SEPARATOR, $wanted_re);
				$wanted_re = str_replace('%s', '[^\r\n]+', $wanted_re);
				$wanted_re = str_replace('%S', '[^\r\n]*', $wanted_re);
				$wanted_re = str_replace('%a', '.+', $wanted_re);
				$wanted_re = str_replace('%A', '.*', $wanted_re);
				$wanted_re = str_replace('%w', '\s*', $wanted_re);
				$wanted_re = str_replace('%i', '[+-]?\d+', $wanted_re);
				$wanted_re = str_replace('%d', '\d+', $wanted_re);
				$wanted_re = str_replace('%x', '[0-9a-fA-F]+', $wanted_re);
				$wanted_re = str_replace('%f', '[+-]?\.?\d+\.?\d*(?:[Ee][+-]?\d+)?', $wanted_re);
				$wanted_re = str_replace('%c', '.', $wanted_re);
				// %f allows two points "-.0.0" but that is the best *simple* expression
				
			}
	/* DEBUG YOUR REGEX HERE
			var_dump($wanted_re);
			print(str_repeat('=', 80) . "\n");
			var_dump($output);
	*/
			$failed = !preg_match("/^$wanted_re\$/s", $output);
		}
		
		$skipexpect = false;
		if (!$failed && $this->conf['TEST_WEB'] && isset($section_text['EXPECTHEADERS'])) {
			$want = array();
			$lines = preg_split("/[\n\r]+/",$section_text['EXPECTHEADERS']);
			$wanted='';
            foreach ($lines as $line) {
                if (strpos($line, ':')!==false) {
                    $line = explode(":", $line, 2);
                    $want[trim($line[0])] = trim($line[1]);
					$wanted .= trim($line[0]).': '.trim($line[1])."\n";
                }
            }
			$output='';
			foreach ($want as $k=>$v) {
				$output .= "$k: {$headers[$k]}\n";
				if (!isset($headers[$k]) || $headers[$k] != $v) {
					$failed = TRUE;
				}
			}
			
			// different servers may do different things on non-200 results
			// for instance, IIS will deliver it's own error pages, so we
			// cannot expect to match up the EXPECT section.  We may however,
			// want to match EXPECT on more than 200 results, so this may
			// need to change later.
			$skipexpect = isset($headers['Status']) && $headers['Status'] != 200;
		}
			
		if (!$failed && !$skipexpect && isset($section_text['EXPECT'])) {
			$wanted = $section_text['EXPECT'];
			$wanted = preg_replace('/\r\n/',"\n",$wanted);
			$failed = (0 != strcmp($output,$wanted));
		}
		
		if (!$failed) {
			@unlink($tmp_file);
			$this->showstatus($tested, 'PASSED');
			return 'PASSED';
		}
			
		// Test failed so we need to report details.
		$this->showstatus($tested, 'FAILED');
	
		$this->failed_tests[] = array(
							'name' => $file,
							'test_name' => $tested,
							'output' => preg_replace('/\.phpt$/','.log', $file),
							'diff'   => preg_replace('/\.phpt$/','.diff', $file)
							);
	
		if ($this->conf['TEST_PHP_DETAILED'])
			$this->writemsg(generate_diff($wanted,$output)."\n");
			
		// write .exp
		if (strpos($this->conf['TEST_PHP_LOG_FORMAT'],'E') !== FALSE) {
			$logname = preg_replace('/\.phpt$/','.exp',$file);
			file_put_contents($logname,$wanted);
		}
	
		// write .out
		if (strpos($this->conf['TEST_PHP_LOG_FORMAT'],'O') !== FALSE) {
			$logname = preg_replace('/\.phpt$/','.out',$file);
			file_put_contents($logname,$output);
		}
	
		// write .diff
		if (strpos($this->conf['TEST_PHP_LOG_FORMAT'],'D') !== FALSE) {
			$logname = preg_replace('/\.phpt$/','.diff',$file);
			file_put_contents($logname,generate_diff($wanted,$output));
		}
	
		// write .log
		if (strpos($this->conf['TEST_PHP_LOG_FORMAT'],'L') !== FALSE) {
			$logname = preg_replace('/\.phpt$/','.log',$file);
			file_put_contents($logname,
						"\n---- EXPECTED OUTPUT\n$wanted\n".
						"---- ACTUAL OUTPUT\n$output\n".
						"---- FAILED\n");
			// display emacs/msvc error output
			if (strpos($this->conf['TEST_PHP_LOG_FORMAT'],'C') !== FALSE) {
				$this->error_report($file,$logname,$tested);
			}
		}
		return 'FAILED';
	}

	//
	//  Write an error in a format recognizable to Emacs or MSVC.
	//
	function error_report($testname,$logname,$tested) 
	{
		$testname = realpath($testname);
		$logname  = realpath($logname);
		switch ($this->conf['TEST_PHP_ERROR_STYLE']) {
		default:
		case 'MSVC':
			$this->writemsg($testname . "(1) : $tested\n");
			$this->writemsg($logname . "(1) :  $tested\n");
			break;
		case 'EMACS':
			$this->writemsg($testname . ":1: $tested\n");
			$this->writemsg($logname . ":1:  $tested\n");
			break;
		}
	}

	function error($message)
	{
		$this->writemsg("ERROR: {$message}\n");
		exit(1);
	}
}

$test = new testHarness();
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
?>
