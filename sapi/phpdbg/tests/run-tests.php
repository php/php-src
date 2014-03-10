<?php
namespace phpdbg\testing {

	/* 
	* Workaround ...
	*/
	if (!defined('DIR_SEP'))
		define('DIR_SEP', '\\' . DIRECTORY_SEPARATOR);
		
	/**
	* TestConfigurationExceptions are thrown 
	* when the configuration prohibits tests executing
	*
	* @package phpdbg
	* @subpackage testing
	*/
	class TestConfigurationException extends \Exception {
		
		/** 
		*
		* @param array Tests confguration
		* @param message Exception message
		* @param ... formatting parameters
		*/
		public function __construct() {
			$argv = func_get_args();
			
			if (count($argv)) {
			
				$this->config = array_shift($argv);
				$this->message = vsprintf(
					array_shift($argv), $argv);
			}
		}
	}
	
	/**
	*
	* @package phpdbg
	* @subpackage testing
	*/
	class TestsConfiguration implements \ArrayAccess {
	
		/**
		*
		* @param array basic configuration
		* @param array argv
		*/
		public function __construct($config, $cmd) {
			$this->options = $config;
			while (($key = array_shift($cmd))) {
				switch (substr($key, 0, 1)) {
					case '-': switch(substr($key, 1, 1)) {
						case '-': {
							$arg = substr($key, 2);
							if (($e=strpos($arg, '=')) !== false) {
								$key = substr($arg, 0, $e);
								$value = substr($arg, $e+1);
							} else {
								$key = $arg;
								$value = array_shift($cmd);
							}
					
							if (isset($key) && isset($value)) {
								switch ($key) {
									case 'phpdbg':
									case 'width':
										$this->options[$key] = $value;
									break;
									
									default: {
										if (isset($config[$key])) {
											if (is_array($config[$key])) {
												$this->options[$key][] = $value;
											} else {
												$this->options[$key] = array($config[$key], $value);
											}
										} else {
											$this->options[$key] = $value;
										}
									}	
								}
								
							}
						} break;
				
						default:
							$this->flags[] = substr($key, 1);
					} break;
				}
			}
			
			if (!is_executable($this->options['phpdbg'])) {
				throw new TestConfigurationException(
					$this->options, 'phpdbg could not be found at the specified path (%s)', $this->options['phpdbg']);
			} else $this->options['phpdbg'] = realpath($this->options['phpdbg']);
			
			$this->options['width'] = (integer) $this->options['width'];
			
			/* display properly, all the time */
			if ($this->options['width'] < 50) {
				$this->options['width'] = 50;
			}
			
			/* calculate column widths */
			$this->options['lwidth'] = ceil($this->options['width'] / 3);
			$this->options['rwidth'] = ceil($this->options['width'] - $this->options['lwidth']) - 5;
		}
		
		public function hasFlag($flag) { 
			return in_array(
				$flag, $this->flags); 
		}
		
		public function offsetExists($offset) 		{ return isset($this->options[$offset]); }
		public function offsetGet($offset)			{ return $this->options[$offset]; }
		public function offsetUnset($offset)		{ unset($this->options[$offset]); }
		public function offsetSet($offset, $data) 	{ $this->options[$offset] = $data; }
		
		protected $options = array();
		protected $flags = array();
	}
	
	/**
	* Tests is the console programming API for the test suite
	*
	* @package phpdbg
	* @subpackage testing
	*/
	class Tests {
	
		/**
		* Construct the console object
		*
		* @param array basic configuration 
		* @param array command line
		*/
		public function __construct(TestsConfiguration &$config) {
			$this->config = &$config;
			
			if ($this->config->hasFlag('help') ||
				$this->config->hasFlag('h')) {
				$this->showUsage();
				exit;
			}
		}
		
		/**
		* Find valid paths as specified by configuration 
		*
		*/
		public function findPaths($in = null) {
			$paths = array();
			$where = ($in != null) ? array($in) : $this->config['path'];
			
			foreach ($where as &$path) {
				if ($path) {
					if (is_dir($path)) {
						$paths[] = $path;
						foreach (scandir($path) as $child) {
							if ($child != '.' && $child != '..') {
								$paths = array_merge(
									$paths, $this->findPaths("$path/$child"));
							}
						}
					}
				}
			}
			
			return $paths;
		}
		
		/**
		*
		* @param string the path to log
		*/
		public function logPath($path) {
			printf(
				'%s [%s]%s', 
				str_repeat(
					'-', $this->config['width'] - strlen($path)), 
				$path, PHP_EOL);
		}
		
		/**
		*
		* @param string the path to log
		*/
		public function logPathStats($path) {
			if (!isset($this->stats[$path])) {
				return;
			}
			
			$total = array_sum($this->stats[$path]);
			
			if ($total) {
				@$this->totals[true] += $this->stats[$path][true];
				@$this->totals[false] += $this->stats[$path][false];
			
				$stats = @sprintf(
					"%d/%d %%%d", 
					$this->stats[$path][true],
					$this->stats[$path][false],
					(100 / $total) * $this->stats[$path][true]);
			
				printf(
					'%s [%s]%s',
					str_repeat(
						' ', $this->config['width'] - strlen($stats)), 
					$stats, PHP_EOL);
			
				printf("%s%s", str_repeat('-', $this->config['width']+3), PHP_EOL);
				printf("%s", PHP_EOL);
			}
		}
		
		/**
		*
		*/
		public function logStats() {
			$total = array_sum($this->totals);
			$stats = @sprintf(
				"%d/%d %%%d",
				$this->totals[true],
				$this->totals[false],
				(100 / $total) * $this->totals[true]);
			printf(
				'%s [%s]%s',
				str_repeat(
					' ', $this->config['width'] - strlen($stats)), 
				$stats, PHP_EOL);
			
		}
		
		/**
		*
		*/
		protected function showUsage() {
			printf('usage: php %s [flags] [options]%s', $this->config['exec'], PHP_EOL);
			printf('[options]:%s', PHP_EOL);
			printf("\t--path\t\tadd a path to scan outside of tests directory%s", PHP_EOL);
			printf("\t--width\t\tset line width%s", PHP_EOL);
			printf("\t--options\toptions to pass to phpdbg%s", PHP_EOL);
			printf("\t--phpdbg\tpath to phpdbg binary%s", PHP_EOL);
			printf('[flags]:%s', PHP_EOL);
			printf("\t-nodiff\t\tdo not write diffs on failure%s", PHP_EOL);
			printf("\t-nolog\t\tdo not write logs on failure%s", PHP_EOL);
			printf('[examples]:%s', PHP_EOL);
			printf("\tphp %s --phpdbg=/usr/local/bin/phpdbg --path=/usr/src/phpdbg/tests --options -n%s", 
				$this->config['exec'], PHP_EOL);
			
		}
		
		/**
		* Find valid tests at the specified path (assumed valid)
		*
		* @param string a valid path
		*/
		public function findTests($path) {
			$tests = array();
			
			foreach (scandir($path) as $file) {
				if ($file == '.' || $file == '..') 
					continue;
			
				$test = sprintf('%s/%s', $path, $file);

				if (preg_match('~\.test$~', $test)) {
					yield new Test($this->config, $test);
				}
			}
		}
		
		/**
		*
		* @param Test the test to log
		*/
		public function logTest($path, Test $test) {
			@$this->stats[$path][($result=$test->getResult())]++;
			
			printf(
				"%-{$this->config['lwidth']}s %-{$this->config['rwidth']}s [%s]%s",
				$test->name, 
				$test->purpose, 
				$result ? "PASS" : "FAIL",
				PHP_EOL);
		}
		
		protected $config;
	}
	
	class Test {
		/*
		* Expect exact line for line match
		*/
		const EXACT =	 	0x00000001;
		
		/*
		* Expect strpos() !== false
		*/
		const STRING = 		0x00000010;
		
		/*
		* Expect stripos() !== false
		*/
		const CISTRING =	0x00000100;
		
		/*
		* Formatted output
		*/
		const FORMAT =		0x00001000;
		
		/**
		* Format specifiers
		*/
		private static $format = array(
			'search' => array(
				'%e',
				'%s',
				'%S',
				'%a',
				'%A',
				'%w',
				'%i',
				'%d',
				'%x',
				'%f',
				'%c',
				'%t',
				'%T'
			),
			'replace' => array(
				DIR_SEP,
				'[^\r\n]+',
				'[^\r\n]*',
				'.+',
				'.*',
				'\s*',
				'[+-]?\d+',
				'\d+',
				'[0-9a-fA-F]+',
				'[+-]?\.?\d+\.?\d*(?:[Ee][+-]?\d+)?',
				'.',
				'\t',
				'\t+'
			)
		);
 		
		/**
		* Constructs a new Test object given a specilized phpdbginit file
		*
		* @param array configuration
		* @param string file
		*/
		public function __construct(TestsConfiguration &$config, &$file) {
			if (($handle = fopen($file, 'r'))) {
				while (($line = fgets($handle))) {
					$trim = trim($line);
					
					switch (substr($trim, 0, 1)) {
						case '#': if (($chunks = array_map('trim', preg_split('~:~', substr($trim, 1), 2)))) {
							if (property_exists($this, $chunks[0])) {
								switch ($chunks[0]) {
									case 'expect': {
										if ($chunks[1]) {
											switch (strtoupper($chunks[1])) {
												case 'TEST::EXACT':
												case 'EXACT': { $this->expect = TEST::EXACT; } break;
												
												case 'TEST::STRING':
												case 'STRING': { $this->expect = TEST::STRING; } break;
												
												case 'TEST::CISTRING':
												case 'CISTRING': { $this->expect = TEST::CISTRING; } break;
												
												case 'TEST::FORMAT':
												case 'FORMAT': { $this->expect = TEST::FORMAT; } break;
												
												default: 
													throw new TestConfigurationException(
														$this->config, "unknown type of expectation (%s)", $chunks[1]);
											}
										}
									} break;
									
									default: {
										$this->$chunks[0] = $chunks[1];
									}	
								}
							} else switch(substr($trim, 1, 1)) {
								case '#': { /* do nothing */ } break;
								
								default: {
									$line = preg_replace(
										"~(\r\n)~", "\n", substr($trim, 1));
									
									$line = trim($line);
									
									switch ($this->expect) {
										case TEST::FORMAT:
											$this->match[] = str_replace(
												self::$format['search'], 
												self::$format['replace'], preg_quote($line));
										break;
										
										default: $this->match[] = $line;
									}
								}
							}
						} break;

						default:
							break 2;
					}
				}
				fclose($handle);
				
				$this->config = &$config;
				$this->file = &$file;
			}
		}
		
		/**
		* Obvious!! 
		* 
		*/
		public function getResult() {
			$options = sprintf(
				'-i%s -qb', $this->file);
			
			if ($this->options) {
				$options = sprintf(
					'%s %s %s',
					$options, 
					$this->config['options'], 
					$this->options
				);
			} else {
				$options = sprintf(
					'%s %s', $options, $this->config['options']
				);
			}

			$result = `{$this->config['phpdbg']} {$options}`;

			if ($result) {
				foreach (preg_split('~(\r|\n)~', $result) as $num => $line) {
					if (!$line && !isset($this->match[$num]))
						continue;
					
					switch ($this->expect) {
						case TEST::EXACT: {
							if (strcmp($line, $this->match[$num]) !== 0) {
								$this->diff['wants'][$num] = &$this->match[$num];
								$this->diff['gets'][$num] = $line;
							}
						} continue 2;

						case TEST::STRING: {
							if (strpos($line, $this->match[$num]) === false) {
								$this->diff['wants'][$num] = &$this->match[$num];
								$this->diff['gets'][$num] = $line;
							}
						} continue 2;
						
						case TEST::CISTRING: {
							if (stripos($line, $this->match[$num]) === false) {
								$this->diff['wants'][$num] = &$this->match[$num];
								$this->diff['gets'][$num] = $line;
							}
						} continue 2;
						
						case TEST::FORMAT: {
							$line = trim($line);
							if (!preg_match("/^{$this->match[$num]}\$/s", $line)) {
								$this->diff['wants'][$num] = &$this->match[$num];
								$this->diff['gets'][$num] = $line;
							}
						} continue 2;
					}
				}
			}
			
			$this->writeLog($result);
			$this->writeDiff();
			
			return (count($this->diff) == 0);
		}
		
		/**
		* Write diff to disk if configuration allows it
		*
		*/
		protected function writeDiff() {
			$diff = sprintf(
				'%s/%s.diff',
				dirname($this->file), basename($this->file));
				
			if (count($this->diff['wants'])) {
				if (!in_array('nodiff', $this->config['flags'])) {
					if (($diff = fopen($diff, 'w+'))) {

						foreach ($this->diff['wants'] as $line => $want) {
							$got = $this->diff['gets'][$line];
						
							fprintf(
								$diff, '(%d) -%s%s', $line+1, $want, PHP_EOL);
							fprintf(
								$diff, '(%d) +%s%s', $line+1, $got, PHP_EOL);
						}

						fclose($diff);
					}
				}
			} else unlink($diff);
		}
		
		/**
		* Write log to disk if configuration allows it
		*
		*/
		protected function writeLog(&$result = null) {
			$log = sprintf(
				'%s/%s.log',
				dirname($this->file), basename($this->file));

			if (count($this->diff) && $result) {
				if (!in_array('nolog', $this->config['flags'])) {
					@file_put_contents(
						$log, $result);
				}
			} else unlink($log);
		}
		
		public $name;
		public $purpose;
		public $file;
		public $options;
		public $expect;
		
		protected $match;
		protected $diff;
		protected $stats;
		protected $totals;
	}
}

namespace {
	use \phpdbg\Testing\Test;
	use \phpdbg\Testing\Tests;
	use \phpdbg\Testing\TestsConfiguration;

	$cwd = dirname(__FILE__);
	$cmd = $_SERVER['argv'];
	{
		$config = new TestsConfiguration(array(
			'exec' => realpath(array_shift($cmd)),
			'phpdbg' => realpath(sprintf(
				'%s/../phpdbg', $cwd
			)),
			'path' => array(
				realpath(dirname(__FILE__))
			),
			'flags' => array(),
			'width' => 75
		), $cmd);

		$tests = new Tests($config);

		foreach ($tests->findPaths() as $path) {	
			$tests->logPath($path);

			foreach ($tests->findTests($path) as $test) {
				$tests->logTest($path, $test);
			}
		
			$tests->logPathStats($path);
		}
		
		$tests->logStats();
	}
}
?>
