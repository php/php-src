<?php

/**
 * @file pharcommand.inc
 * @ingroup Phar
 * @brief class CLICommand
 * @author  Marcus Boerger
 * @date    2007 - 2008
 *
 * Phar Command
 */
// {{{ class PharCommand extends CLICommand
/**
 * PharCommand class
 *
 * This class handles the handling of the phar
 * commands. It will be used from command line/console
 * in order to retrieve and execute phar functions.
 *
 * @ingroup Phar
 * @brief   Phar console command implementation
 * @author  Marcus Boerger
 * @version 1.0
 */
class PharCommand extends CLICommand
{
	// {{{ public function cli_get_SP2
	public function cli_get_SP2($l1, $arg_inf)
	{
		return str_repeat(' ', $l1 + 2 + 4 + 9);
	}
	// }}}
	// {{{ public function cli_get_SP3
	/**
	 * Cli Get SP3
	 *
	 * @param string $l1      Eleven
	 * @param string $l2      Twelve
	 * @param string $arg_inf
	 * @return string  The repeated string.
	 */
	function cli_get_SP3($l1, $l2, $arg_inf)
	{
		return str_repeat(' ', $l1 + 2 + 4 + 9 + 2 + $l2 + 2);
	}
	// }}}
	// {{{ static function phar_args
	/**
	 * Phar arguments
	 *
	 * This function contains all the phar commands
	 *
	 * @param  string $which    Which argument is chosen.
	 * @param  string $phartype The type of phar, specific file to work on
	 * @return unknown
	 */
	static function phar_args($which, $phartype)
	{
		$phar_args = array(
			'a' => array(
				'typ' => 'alias',
				'val' => NULL,
				'inf' => '<alias>  Provide an alias name for the phar file.'
			),
			'b' => array(
				'typ' => 'any',
				'val' => NULL,
				'inf' => '<bang>   Hash-bang line to start the archive (e.g. #!/usr/bin/php). The hash '
						 .'         mark itself \'#!\' and the newline character are optional.'
			),
			'c' => array(
				'typ' => 'compalg',
				'val' => NULL,
				'inf' => '<algo>   Compression algorithm.',
				'select' => array(
					'0'    => 'No compression',
					'none' => 'No compression',
					'auto' => 'Automatically select compression algorithm'
				)
			),
			'e' => array(
				'typ' => 'entry',
				'val' => NULL,
				'inf' => '<entry>  Name of entry to work on (must include PHAR internal directory name if any).'
			),
			'f' => array(
				'typ' => $phartype,
				'val' => NULL,
				'inf' => '<file>   Specifies the phar file to work on.'
			),
			'h' => array(
				'typ' => 'select',
				'val' => NULL,
				'inf' => '<method> Selects the hash algorithm.',
				'select' => array('md5' => 'MD5','sha1' => 'SHA1')
			),
			'i' => array(
				'typ' => 'regex',
				'val' => NULL,
				'inf' => '<regex>  Specifies a regular expression for input files.'
			),
			'k' => array(
				'typ' => 'any',
				'val' => NULL,
				'inf' => '<index>  Subscription index to work on.',
			),
			'l' => array(
				'typ' => 'int',
				'val' => 0,
				'inf' => '<level>  Number of preceding subdirectories to strip from file entries',
			),
			'm' => array(
				'typ' => 'any',
				'val' => NULL,
				'inf' => '<meta>   Meta data to store with entry (serialized php data).'
			),
			'p' => array(
				'typ' => 'loader',
				'val' => NULL,
				'inf' => '<loader> Location of PHP_Archive class file (pear list-files PHP_Archive).'
						 .'You can use \'0\' or \'1\' to locate it automatically using the mentioned '
						 .'pear command. When using \'0\' the command does not error out when the '
						 .'class file cannot be located. This switch also adds some code around the '
						 .'stub so that class PHP_Archive gets registered as phar:// stream wrapper '
						 .'if necessary. And finally this switch will add the file phar.inc from '
						 .'this package and load it to ensure class Phar is present.'
						 ,
			),
			's' => array(
				'typ' => 'file',
				'val' => NULL,
				'inf' => '<stub>   Select the stub file.'
			),
			'x' => array(
				'typ' => 'regex',
				'val' => NULL,
				'inf' => '<regex>  Regular expression for input files to exclude.'
			),
			'y' => array(
				'typ' => 'privkey',
				'val' => NULL,
				'inf' => '<key>    Private key for OpenSSL signing.',
			),
		);

		if (extension_loaded('zlib')) {
			$phar_args['c']['select']['gz']    = 'GZip compression';
			$phar_args['c']['select']['gzip']  = 'GZip compression';
		}

		if (extension_loaded('bz2')) {
			$phar_args['c']['select']['bz2']   = 'BZip2 compression';
			$phar_args['c']['select']['bzip2'] = 'BZip2 compression';
		}

		$hash_avail = Phar::getSupportedSignatures();
		$hash_optional = array('SHA-256' => 'SHA256',
							   'SHA-512' => 'SHA512',
							   'OpenSSL' => 'OpenSSL');
		if (!in_array('OpenSSL', $hash_avail)) {
			unset($phar_args['y']);
		}

		foreach($hash_optional as $key => $name) {
			if (in_array($key, $hash_avail)) {
				$phar_args['h']['select'][strtolower($name)] = $name;
			}
		}

		$args = array();

		foreach($phar_args as $lkey => $cfg) {
			$ukey     = strtoupper($lkey);
			$required = strpos($which, $ukey) !== false;
			$optional = strpos($which, $lkey) !== false;

			if ($required || $optional) {
				$args[$lkey] = $cfg;
				$args[$lkey]['required'] = $required;
			}
		}
		return $args;
	}
	// }}}
	// {{{ static function strEndsWith
	/**
	 * String Ends With
	 *
	 * Whether a string ends with another needle.
	 *
	 * @param string $haystack  The haystack
	 * @param string $needle    The needle.
	 * @return mixed false if doesn't end with anything, the string
	 *               substr'ed if the string ends with the needle.
	 */
	static function strEndsWith($haystack, $needle)
	{
		return substr($haystack, -strlen($needle)) == $needle;
	}
	// }}}
	// {{{ static function cli_arg_typ_loader
	/**
	 * Argument type loader
	 *
	 * @param string $arg   Either 'auto', 'optional' or an filename that
	 *                      contains class PHP_Archive
	 * @param  string $cfg  Configuration to pass to a new file
	 * @param  string $key  The key
	 * @return string $arg  The argument.
	 */
	static function cli_arg_typ_loader($arg, $cfg, $key)
	{
		if (($arg == '0' || $arg == '1') && !file_exists($arg) && substr(PHP_OS, 0, 3) != 'WIN') {
			$found = NULL;
			$apiver = false;
			$path = explode(PATH_SEPARATOR, $_ENV['PATH']);
			$pear = false;
			foreach ($path as $component) {
				if (file_exists($component . DIRECTORY_SEPARATOR . 'pear')
					&& is_executable($component . DIRECTORY_SEPARATOR . 'pear')) {
					$pear = true;
					break;
				}
			}
			if ($pear) {
				$apiver = `pear -q info PHP_Archive 2>/dev/null|grep 'API Version'`;
				$apiver = trim(substr($apiver, strlen('API Version')));
			}
			if ($apiver) {
				self::notice("PEAR package PHP_Archive: API Version: $apiver.\n");
				$files  = explode("\n", `pear list-files PHP_Archive`);
				$phpdir = `pear config-get php_dir 2>/dev/null`;
				$phpdir = trim($phpdir);
				self::notice("PEAR package PHP_Archive: $phpdir.\n");
				if (is_dir($phpdir)) {
					foreach($files as $ent) {
						$matches = NULL;
						if (preg_match(",^php[ \t]+([^ \t].*[\\\\/]PHP[\\\\/]Archive\.php)$,", $ent, $matches)) {
							$sub = $matches[1];
							if (strpos($sub, $phpdir) !== 0) {
								$found = NULL;
								break;
							}
							$found = $sub;
							break;
						}
					}
				} else {
					self::notice("PEAR package PHP_Archive: corrupt or inaccessible base dir: $php_dir.\n");
				}
			}
			if (isset($found)) {
				self::notice("PEAR package PHP_Archive: $found.\n");
			} else {
				$msg = "PEAR package PHP_Archive not installed: generated phar will require PHP's phar extension be enabled.\n";
				if ($arg == '0') {
					self::notice($msg);
				} else {
					self::error($msg);
				}
			}
			$arg = $found;
		}
		return self::cli_arg_typ_file($arg);
	}
	// }}}
	// {{{ static function cli_arg_typ_pharnew
	/**
	 * Argument type new phar
	 *
	 * @param  string $arg  The new phar component.
	 * @param  string $cfg  Configuration to pass to a new file
	 * @param  string $key  The key
	 * @return string $arg  The new argument file.
	 */
	static function cli_arg_typ_pharnew($arg, $cfg, $key)
	{
		$arg = self::cli_arg_typ_filenew($arg, $cfg, $key);
		if (!Phar::isValidPharFilename($arg)) {
			self::error("Phar files must have file extension '.phar', '.phar.php', '.phar.bz2' or '.phar.gz'.\n");
		}
		return $arg;
	}
	// }}}
	// {{{ static function cli_arg_typ_pharfile
	/**
	 * Argument type existing Phar file
	 *
	 * Return filename of an existing Phar.
	 *
	 * @param  string $arg      The file in the phar to open.
	 * @param  string $cfg      The configuration information
	 * @param  string $key      The key information.
	 * @return string $pharfile The name of the loaded Phar file.
	 * @note The Phar will be loaded
	 */
	static function cli_arg_typ_pharfile($arg, $cfg, $key)
	{
		try {
			$pharfile = self::cli_arg_typ_file($arg, $cfg, $key);

			if (!Phar::loadPhar($pharfile)) {
				self::error("Unable to open phar '$arg'\n");
			}

			return $pharfile;
		} catch(Exception $e) {
			self::error("Exception while opening phar '$arg':\n" . $e->getMessage() . "\n");
		}
	}
	// }}}
	// {{{ static function cli_arg_typ_pharurl
	/**
	 * Argument type Phar url-like
	 *
	 * Check the argument as cli_arg_Typ_phar and return its name prefixed
	 * with phar://
	 *
	 * Ex:
	 * <code>
	 *  $arg = 'pharchive.phar/file.php';
	 *  cli_arg_typ_pharurl($arg)
	 * </code>
	 *
	 * @param  string $arg The url-like phar archive to retrieve.
	 * @return string The phar file-archive.
	 */
	static function cli_arg_typ_pharurl($arg, $cfg, $key)
	{
		return 'phar://' . self::cli_arg_typ_pharfile($arg, $cfg, $key);
	}
	// }}}
	// {{{ static function cli_arg_typ_phar
	/**
	 * Cli argument type phar
	 *
	 * @param  string $arg  The phar archive to use.
	 * @return object new Phar of the passed argument.
	 */
	static function cli_arg_typ_phar($arg, $cfg, $key)
	{
		try {
			return new Phar(self::cli_arg_typ_pharfile($arg, $cfg, $key));
		} catch(Exception $e) {
			self::error("Exception while opening phar '$argv':\n" . $e->getMessage() . "\n");
		}
	}
	// }}}
	// {{{ static function cli_arg_typ_entry
	/**
	 * Argument type Entry name
	 *
	 * @param  string $arg The argument (the entry)
	 * @return string $arg The entry itself.
	 */
	static function cli_arg_typ_entry($arg, $cfg, $key)
	{
		// no further check atm, maybe check for no '/' at beginning
		return $arg;
	}
	// }}}
	// {{{ static function cli_arg_typ_compalg
	/**
	 * Argument type compression algorithm
	 *
	 * @param  string $arg  The phar selection
	 * @param  string $cfg  The config option.
	 * @param  string $key  The key information.
	 * @return string $arg  The selected algorithm
	 */
	static function cli_arg_typ_compalg($arg, $cfg, $key)
	{
		$arg = self::cli_arg_typ_select($arg, $cfg, $key);

		switch($arg) {
			case 'auto':
				if (extension_loaded('zlib')) {
					$arg = 'gz';
				} elseif (extension_loaded('bz2')) {
					$arg = 'bz2';
				} else {
					$arg = '0';
				}
				break;
		}
		return $arg;
	}
	// }}}
	// {{{ static function cli_arg_typ_privkey
	/**
	 * Argument type private key (for OpenSSL signing)
	 *
	 * @param  string $arg  The phar selection
	 * @param  string $cfg  The config option.
	 * @param  string $key  The key information.
	 * @return string $arg  The private key.
	 */
	static function cli_arg_typ_privkey($arg, $cfg, $key)
	{
		$arg = self::cli_arg_typ_filecont($arg, $cfg, $key);

		$hash_avail = Phar::getSupportedSignatures();
		if ($arg && !in_array('OpenSSL', $hash_avail))
		{
			self::error("Cannot specifiy private key without OpenSSL support.\n");
		}
		return $arg;
	}
	// }}}
	// {{{ static function phar_check_hash
	/**
	 * Check whether hash method is valid.
	 *
	 * @return Hash constant to be used.
	 */
	function phar_check_hash($hash, $privkey)
	{
		switch($hash) {
			case 'md5':
				return Phar::MD5;
			case 'sha1':
				return Phar::SHA1;
			case 'sha256':
				return Phar::SHA256;
			case 'sha512':
				return Phar::SHA512;
			case 'openssl':
				if (!$privkey) {
					self::error("Cannot use OpenSSL signing without key.\n");
				}
				return Phar::OPENSSL;
		}
	}
	// }}}
	// {{{ static function cli_cmd_inf_pack
	/**
	 * Information pack
	 *
	 * @return string A description about packing files into a Phar archive.
	 */
	static function cli_cmd_inf_pack()
	{
		return "Pack files into a PHAR archive.\n" .
			   "When using -s <stub>, then the stub file is being " .
			   "excluded from the list of input files/dirs." .
			   "To create an archive that contains PEAR class PHP_Archive " .
			   "then point -p argument to PHP/Archive.php.\n";
	}
	// }}}
	// {{{ static function cli_cmd_arg_pack
	/**
	 * Pack a new phar infos
	 *
	 * @return array  $args  The arguments for a new Phar archive.
	 */
	static function cli_cmd_arg_pack()
	{
		$args = self::phar_args('abcFhilpsxy', 'pharnew');

		$args[''] = array(
			'typ'     => 'any',
			'val'      => NULL,
			'required' => 1,
			'inf'      => '         Any number of input files and directories. If -i is in use then ONLY files and matching the given regular expression are being packed. If -x is given then files matching that regular expression are NOT being packed.',
		);

		return $args;
	}
	// }}}
	// {{{ function phar_set_stub_begin
	/**
	 * Set the stub
	 */
	public function phar_set_stub_begin(Phar $phar, $stub, $loader = NULL, $hashbang = NULL)
	{
		if (isset($stub)) {
			$c = file_get_contents($stub);

			if (substr($c, 0, 2) == '#!') {
				if (strpos($c, "\n") !== false) {
					if (!isset($hashbang)) {
						$hashbang = substr($c, 0, strpos($c, "\n") + 1);
					}
					$c = substr($c, strpos($c, "\n") + 1);
				} else {
					if (!isset($hashbang)) {
						$hashbang = $c;
					}
					$c = NULL;
				}
			}

			if (isset($hashbang)) {
				if (substr($hashbang, 0, 2) != '#!') {
					$hashbang = '#!' . $hashbang;
				}
				if (substr($hashbang, -1) != "\n") {
					$hashbang .= "\n";
				}
			} else {
				$hashbang = "";
			}

			if (isset($loader)) {
				$s = "<?php if (!class_exists('PHP_Archive')) {\n?>";
				if (is_file($loader)) {
					$s .= file_get_contents($loader);
				}
				$s .= "<?php\n";
				$s .= "}\n";
				$s .= "if (!in_array('phar', stream_get_wrappers())) {\n";
				$s .= "\tstream_wrapper_register('phar', 'PHP_Archive');\n";
				$s .= "}\n";
				$s .= "if (!class_exists('Phar',0)) {\n";
				$s .= "\tinclude 'phar://'.__FILE__.'/phar.inc';\n";
				$s .= "}\n";
				$s .= '?>';
				$s .= $c;

				$phar->setStub($hashbang . $s);
			} else {
				$phar->setStub($hashbang . $c);
			}
			return new SplFileInfo($stub);
		}
		return NULL;
	}
	// }}}
	// {{{ function phar_set_stub_end
	/**
	 * Set stub end
	 */
	public function phar_set_stub_end(Phar $phar, $stub, $loader = NULL)
	{
		if (isset($stub) && isset($loader)) {
			if (substr(__FILE__, -15) == 'pharcommand.inc') {
				self::phar_add_file($phar, 0, 'phar.inc', 'phar://'.__FILE__.'/phar.inc', NULL);
			} else {
				self::phar_add_file($phar, 0, 'phar.inc', dirname(__FILE__).'/phar/phar.inc', NULL);
			}
		}
	}
	// }}}
	// {{{ function cli_cmd_run_pack
	/**
	 * Pack a new Phar
	 *
	 * This function will try to pack a new Phar archive.
	 *
	 * @see Exit to make sure that we are done.
	 */
	public function cli_cmd_run_pack()
	{
		if (ini_get('phar.readonly')) {
			self::error("Creating phar files is disabled by ini setting 'phar.readonly'.\n");
		}

		if (!Phar::canWrite()) {
			self::error("Creating phar files is disabled, Phar::canWrite() returned false.\n");
		}

		$alias    = $this->args['a']['val'];
		$hashbang = $this->args['b']['val'];
		$archive  = $this->args['f']['val'];
		$hash     = $this->args['h']['val'];
		$privkey  = $this->args['y']['val'];
		$regex    = $this->args['i']['val'];
		$level    = $this->args['l']['val'];
		$loader   = $this->args['p']['val'];
		$stub     = $this->args['s']['val'];
		$invregex = $this->args['x']['val'];
		$input    = $this->args['']['val'];

		$hash = self::phar_check_hash($hash, $privkey);

		$phar  = new Phar($archive, 0, $alias);

		$phar->startBuffering();

		$stub = $this->phar_set_stub_begin($phar, $stub, $loader, $hashbang);

		if (!is_array($input)) {
			$this->phar_add($phar, $level, $input, $regex, $invregex, $stub, NULL, isset($loader));
		} else {
			foreach($input as $i) {
				$this->phar_add($phar, $level, $i, $regex, $invregex, $stub, NULL, isset($loader));
			}
		}

		$this->phar_set_stub_end($phar, $stub, $loader);

		switch($this->args['c']['val']) {
			case 'gz':
			case 'gzip':
				$phar->compressFiles(Phar::GZ);
				break;
			case 'bz2':
			case 'bzip2':
				$phar->compressFiles(Phar::BZ2);
				break;
			default:
				$phar->decompressFiles();
				break;
		}

		if ($hash) {
			$phar->setSignatureAlgorithm($hash, $privkey);
		}

		$phar->stopBuffering();
		exit(0);
	}
	// }}}
	// {{{ static function phar_add
	/**
	 * Add files to a phar archive.
	 *
	 * This function will take a directory and iterate through
	 * it and get the files to insert into the Phar archive.
	 *
	 * @param Phar        $phar      The phar object.
	 * @param string      $input     The input directory
	 * @param string      $regex     The regex used in RegexIterator.
	 * @param string      $invregex  The InvertedRegexIterator expression.
	 * @param SplFileInfo $stub Stub file object
	 * @param mixed       $compress  Compression algorithm or NULL
	 * @param boolean     $noloader  Whether to prevent adding the loader
	 */
	static function phar_add(Phar $phar, $level, $input, $regex, $invregex, SplFileInfo $stub = NULL, $compress = NULL, $noloader = false)
	{
		if ($input && is_file($input) && !is_dir($input)) {
			return self::phar_add_file($phar, $level, $input, $input, $compress);
		}
		$dir   = new RecursiveDirectoryIterator($input);
		$dir   = new RecursiveIteratorIterator($dir);

		if (isset($regex)) {
			$dir = new RegexIterator($dir, $regex);
		}

		if (isset($invregex)) {
			$dir = new InvertedRegexIterator($dir, $invregex);
		}

		try {
			foreach($dir as $file) {
				if ((empty($stub) || $file->getRealPath() != $stub->getRealPath()) && !is_dir($file)) {
					self::phar_add_file($phar, $level, $dir->getSubPathName(), $file, $compress, $noloader);
				}
			}
		} catch(Excpetion $e) {
			self::error("Unable to complete operation on file '$file'\n" . $e->getMessage() . "\n");
		}
	}
	// }}}
	// {{{ static function phar_add_file
	/**
	 * Add a phar file
	 *
	 * This function adds a file to a phar archive.
	 *
	 * @param Phar    $phar      The phar object
	 * @param string  $level     The level of the file.
	 * @param string  $entry     The entry point
	 * @param string  $file      The file to add to the archive
	 * @param string  $compress  The compression scheme for the file.
	 * @param boolean $noloader  Whether to prevent adding the loader
	 */
	static function phar_add_file(Phar $phar, $level, $entry, $file, $compress, $noloader = false)
	{
		$entry = str_replace('//', '/', $entry);
		while($level-- > 0 && ($p = strpos($entry, '/')) !== false) {
			$entry = substr($entry, $p+1);
		}

	if ($noloader && $entry == 'phar.inc') {
		return;
	}

		echo "$entry\n";

		$phar[$entry] = file_get_contents($file);
		switch($compress) {
			case 'gz':
			case 'gzip':
				$phar[$entry]->compress(Phar::GZ);
				break;
			case 'bz2':
			case 'bzip2':
				$phar[$entry]->compress(Phar::BZ2);
				break;
			case '0':
				$phar[$entry]->decompress();
				break;
			default:
				break;
		}
	}
	// }}}
	// {{{ public function phar_dir_echo
	/**
	 * Echo directory
	 *
	 * @param string $pn
	 * @param unknown_type $f
	 */
	public function phar_dir_echo($pn, $f)
	{
		echo "$f\n";
	}
	// }}}
	// {{{ public function phar_dir_operation
	/**
	 * Directory operations
	 *
	 * Phar directory operations.
	 *
	 * @param RecursiveIteratorIterator $dir  The recursiveIteratorIterator object.
	 * @param string                    $func Function to call on the iterations
	 * @param array                     $args Function arguments.
	 */
	public function phar_dir_operation(RecursiveIteratorIterator $dir, $func, array $args = array())
	{
		$regex   = $this->args['i']['val'];
		$invregex= $this->args['x']['val'];

		if (isset($regex)) {
			$dir = new RegexIterator($dir, $regex);
		}

		if (isset($invregex)) {
			$dir = new InvertedRegexIterator($dir, $invregex);
		}

		$any = false;
		foreach($dir as $pn => $f) {
			$any = true;
			call_user_func($func, $pn, $f, $args);
		}
		return $any;
	}
	// {{{ static function cli_cmd_inf_list
	/**
	 * Cli Command Info List
	 *
	 * @return string What inf does
	 */
	static function cli_cmd_inf_list()
	{
		return "List contents of a PHAR archive.";
	}
	// }}}
	// {{{ static function cli_cmd_arg_list
	/**
	 * Cli Command Argument List
	 *
	 * @return arguments list
	 */
	static function cli_cmd_arg_list()
	{
		return self::phar_args('Fix', 'pharurl');
	}
	// }}}
	// {{{ public function cli_cmd_run_list
	/**
	 * Cli Command Run List
	 *
	 * @see $this->phar_dir_operation
	 */
	public function cli_cmd_run_list()
	{
		$this->phar_dir_operation(
			new DirectoryTreeIterator(
				$this->args['f']['val']),
				array($this, 'phar_dir_echo')
			);
	}
	// }}}
	// {{{ static function cli_command_inf_tree
	/**
	 * Cli Command Inf Tree
	 *
	 * @return string  The description of a directory tree for a Phar archive.
	 */
	static function cli_cmd_inf_tree()
	{
		return "Get a directory tree for a PHAR archive.";
	}
	// }}}
	// {{{ static function cli_cmd_arg_tree
	/**
	 * Cli Command Argument Tree
	 *
	 * @return string Arguments in URL format.
	 */
	static function cli_cmd_arg_tree()
	{
		return self::phar_args('Fix', 'pharurl');
	}
	// }}}
	// {{{ public function cli_cmd_run_tree
	/**
	 * Cli Command Run Tree
	 *
	 * Set the phar_dir_operation with a directorygraphiterator.
	 *
	 * @see DirectoryGraphIterator
	 * @see $this->phar_dir_operation
	 *
	 */
	public function cli_cmd_run_tree()
	{
		$a = $this->phar_dir_operation(
			new DirectoryGraphIterator(
				$this->args['f']['val']),
				array($this, 'phar_dir_echo')
			);
		if (!$a) {
			echo "|-<root directory>\n";
		}
	}
	// }}}
	// {{{ cli_cmd_inf_extract
	/**
	 * Cli Command Inf Extract
	 *
	 * @return string The description of the command extra to a directory.
	 */
	static function cli_cmd_inf_extract()
	{
		return "Extract a PHAR package to a directory.";
	}
	// }}}
	// {{{ static function cli_cmd_arg_extract
	/**
	 * Cli Command Arguments Extract
	 *
	 * The arguments for the extract function.
	 *
	 * @return array  The arguments for the extraction.
	 */
	static function cli_cmd_arg_extract()
	{
		$args = self::phar_args('Fix', 'phar');

		$args[''] = array(
			'type' => 'dir',
			'val' => '.',
			'inf' => '         Directory to extract to (defaults to \'.\').',
		);

		return $args;
	}
	// }}}
	// {{{ public function cli_cmd_run_extract
	/**
	 * Run Extract
	 *
	 * Run the extraction of a phar Archive.
	 *
	 * @see $this->phar_dir_operation
	 */
	public function cli_cmd_run_extract()
	{
		$dir = $this->args['']['val'];

		if (is_array($dir)) {
			if (count($dir) != 1) {
				self::error("Only one target directory allowed.\n");
			} else {
				$dir = $dir[0];
			}
		}

		$phar = $this->args['f']['val'];
		$base = $phar->getPathname();
		$bend = strpos($base, '.phar');
		$bend = strpos($base, '/', $bend);
		$base = substr($base, 0, $bend + 1);
		$blen = strlen($base);

		$this->phar_dir_operation(
			new RecursiveIteratorIterator($phar),
			array($this, 'phar_dir_extract'),
			array($blen, $dir)
		);
	}
	// }}}
	// {{{ public function phar_dir_extract
	/**
	 * Extract to a directory
	 *
	 * This function will extract the content of a Phar
	 * to a directory and create new files and directories
	 * depending on the permissions on that folder.
	 *
	 * @param string $pn
	 * @param string $f     The file name
	 * @param array $args   The directory and Blen informations
	 */
	public function phar_dir_extract($pn, $f, $args)
	{
		$blen   = $args[0];
		$dir    = $args[1];
		$sub    = substr($pn, $blen);
		$target = $dir . '/' . $sub;

		if (!file_exists(dirname($target))) {
			@mkdir(dirname($target), 0777, true);
		}
		if (!file_exists(dirname($target))) {
			self::error("Operation could not be completed\n");
		}

		echo "$sub";

		if (!@copy($f, $target)) {
			echo " ...error\n";
		} else {
			echo " ...ok\n";
		}
	}
	// }}}
	// {{{ static function cli_cmd_inf_delete
	/**
	 * Delete an entry from a phar information.
	 *
	 * @return string The information
	 */
	static function cli_cmd_inf_delete()
	{
		return 'Delete entry from a PHAR archive';
	}
	// }}}
	// {{{ static function cli_cmd_arg_delete
	/**
	 * The cli command argument for deleting.
	 *
	 * @return array informations about the arguments to use.
	 */
	static function cli_cmd_arg_delete()
	{
		return self::phar_args('FE', 'phar');
	}
	// }}}
	// {{{ public function cli_cmd_run_delete
	/**
	 * Deleting execution
	 *
	 * Execute the deleting of the file from the phar archive.
	 */
	public function cli_cmd_run_delete()
	{
		$phar  = $this->args['f']['val'];
		$entry = $this->args['e']['val'];

		$phar->startBuffering();
		unset($phar[$entry]);
		$phar->stopBuffering();
	}
	// }}}
	// {{{ static function cli_cmd_inf_add
	/**
	 * Client comment add file information
	 *
	 * @return string The description of the feature
	 */
	static function cli_cmd_inf_add()
	{
		return "Add entries to a PHAR package.";
	}
	// }}}
	// {{{ static function cli_cmd_arg_add
	/**
	 * Add a file arguments
	 */
	static function cli_cmd_arg_add()
	{
		$args = self::phar_args('acFilx', 'phar');
		$args[''] = array(
			'type'     => 'any',
			'val'      => NULL,
			'required' => 1,
			'inf'      => '         Any number of input files and directories. If -i is in use then ONLY files and matching the given regular expression are being packed. If -x is given then files matching that regular expression are NOT being packed.',
		);
		return $args;
	}
	// }}}
	// {{{ public functio cli_cmd_run_add
	/**
	 * Add a file
	 *
	 * Run the action of adding a file to
	 * a phar archive.
	 */
	public function cli_cmd_run_add()
	{
		$compress= $this->args['c']['val'];
		$phar    = $this->args['f']['val'];
		$regex   = $this->args['i']['val'];
		$level   = $this->args['l']['val'];
		$invregex= $this->args['x']['val'];
		$input   = $this->args['']['val'];

		$phar->startBuffering();

		if (!is_array($input)) {
			$this->phar_add($phar, $level, $input, $regex, $invregex, NULL, $compress);
		} else {
			foreach($input as $i) {
				$this->phar_add($phar, $level, $i, $regex, $invregex, NULL, $compress);
			}
		}
		$phar->stopBuffering();
		exit(0);
	}
	// }}}
	// {{{ public function cli_cmd_inf_stub_set
	/**
	 * Set the stup of a phar file.
	 *
	 * @return string The stub set description.
	 */
	public function cli_cmd_inf_stub_set()
	{
		return "Set the stub of a PHAR file. " .
			   "If no input file is specified as stub then stdin is being used.";
	}
	// }}}
	// {{{ public function cli_cmd_arg_stub_set
	/**
	 * Set the argument stub
	 *
	 * @return string arguments for a stub
	 */
	public function cli_cmd_arg_stub_set()
	{
		$args = self::phar_args('bFps', 'phar');
		$args['s']['val'] = 'php://stdin';
		return $args;
	}
	// }}}
	// {{{ public function cli_cmd_run_stub_set
	/**
	 * Cli Command run stub set
	 *
	 * @see   $phar->setStub()
	 */
	public function cli_cmd_run_stub_set()
	{
		$hashbang = $this->args['b']['val'];
		$phar     = $this->args['f']['val'];
		$stub     = $this->args['s']['val'];
		$loader   = $this->args['p']['val'];

		$this->phar_set_stub_begin($phar, $stub, $loader, $hashbang);
		$this->phar_set_stub_end($phar, $stub, $loader);
	}
	// }}}
	// {{{ public function cli_cmd_inf_stub_get
	/**
	 * Get the command stub infos.
	 *
	 * @return string a description of the stub of a Phar file.
	 */
	public function cli_cmd_inf_stub_get()
	{
		return "Get the stub of a PHAR file. " .
			   "If no output file is specified as stub then stdout is being used.";
	}
	// }}}
	// {{{ public function cli_cmd_arg_stub_get
	/**
	 * Get the argument stub
	 *
	 * @return array $args The arguments passed to the stub.
	 */
	public function cli_cmd_arg_stub_get()
	{
		$args = self::phar_args('Fs', 'phar');
		$args['s']['val'] = 'php://stdin';
		return $args;
	}
	// }}}
	// {{{ public function cli_cmd_run_stub_get
	/**
	 * Cli Command Run Stub
	 *
	 * Get arguments and store them into a stub.
	 *
	 * @param arguments $args
	 * @see   $this->args
	 */
	public function cli_cmd_run_stub_get($args)
	{
		$phar = $this->args['f']['val'];
		$stub = $this->args['s']['val'];

		file_put_contents($stub, $phar->getStub());
	}
	// }}}
	// {{{ public function cli_cmd_inf_compress
	/**
	 * Cli Command Inf Compress
	 *
	 * Cli Command compress informations
	 *
	 * @return string A description of the command.
	 */
	public function cli_cmd_inf_compress()
	{
		return "Compress or uncompress all files or a selected entry.";
	}
	// }}}
	// {{{ public function cli_cmd_arg_cmpress
	/**
	 * Cli Command Arg Compress
	 *
	 * @return array The arguments for compress
	 */
	public function cli_cmd_arg_compress()
	{
		return self::phar_args('FCe', 'phar');
	}
	// }}}
	// {{{ public function cli_cmd_run_compress
	/**
	 * Cli Command Run Compress
	 *
	 * @see $this->args
	 */
	public function cli_cmd_run_compress()
	{
		$phar  = $this->args['f']['val'];
		$entry = $this->args['e']['val'];

		switch($this->args['c']['val']) {
			case 'gz':
			case 'gzip':
				if (isset($entry)) {
					$phar[$entry]->compress(Phar::GZ);
				} else {
					$phar->compressFiles(Phar::GZ);
				}
				break;
			case 'bz2':
			case 'bzip2':
				if (isset($entry)) {
					$phar[$entry]->compress(Phar::BZ2);
				} else {
					$phar->compressFiles(Phar::BZ2);
				}
				break;
			default:
				if (isset($entry)) {
					$phar[$entry]->decompress();
				} else {
					$phar->decompressFiles();
				}
				break;
		}
	}
	// }}}
	// {{{ public function cli_cmd_inf_sign
	/**
	 * Cli Command Info Signature
	 *
	 * @return string A description of the signature arguments.
	 */
	public function cli_cmd_inf_sign()
	{
		return "Set signature hash algorithm.";
	}
	// }}}
	// {{{ public function cli_cmd_arg_sign
	/**
	 * Cli Command Argument Sign
	 *
	 * @return array Arguments for Signature
	 */
	public function cli_cmd_arg_sign()
	{
		return self::phar_args('FHy', 'phar');
	}
	// }}}
	// {{{ public function cli_cmd_run_sign
	/**
	 * Cli Command Run Signature
	 *
	 * @see $phar->setSignaturealgorithm
	 */
	public function cli_cmd_run_sign()
	{
		$phar     = $this->args['f']['val'];
		$hash     = $this->args['h']['val'];
		$privkey  = $this->args['y']['val'];

		$hash = self::phar_check_hash($hash, $privkey);

		$phar->setSignatureAlgorithm($hash, $privkey);
	}
	// }}}
	// {{{ public function cli_cmd_inf_meta_set
	/**
	 * Cli Command Inf Meta Set
	 *
	 * @return string A description
	 */
	public function cli_cmd_inf_meta_set()
	{
		return "Set meta data of a PHAR entry or a PHAR package using serialized input. " .
			   "If no input file is specified for meta data then stdin is being used." .
			   "You can also specify a particular index using -k. In that case the metadata is " .
			   "expected to be an array and the value of the given index is being set. If " .
			   "the metadata is not present or empty a new array will be created. If the " .
			   "metadata is present and a flat value then the return value is 1. Also using -k " .
			   "the input is been taken directly rather then being serialized.";
	}
	// }}}
	// {{{ public function cli_cmd_arg_meta_set
	/**
	 * Cli Command Argument Meta Set
	 *
	 * @return array  The arguments for meta set
	 */
	public function cli_cmd_arg_meta_set()
	{
		return self::phar_args('FekM', 'phar');
	}
	// }}}
	// {{{ public function cli_cmd_run_met_set
	/**
	 * Cli Command Run Metaset
	 *
	 * @see $phar->startBuffering
	 * @see $phar->setMetadata
	 * @see $phar->stopBuffering
	 */
	public function cli_cmd_run_meta_set()
	{
		$phar  = $this->args['f']['val'];
		$entry = $this->args['e']['val'];
		$index = $this->args['k']['val'];
		$meta  = $this->args['m']['val'];

		$phar->startBuffering();

		if (isset($index)) {
			if (isset($entry)) {
				if ($phar[$entry]->hasMetadata()) {
					$old = $phar[$entry]->getMetadata();
				} else {
					$old = array();
				}
			} else {
				if ($phar->hasMetadata()) {
					$old = $phar->getMetadata();
				} else {
					$old = array();
				}
			}

			if (!is_array($old)) {
				self::error('Metadata is a flat value while an index operation was issued.');
			}

			$old[$index] = $meta;
			$meta = $old;
		} else {
			$meta = unserialize($meta);
		}

		if (isset($entry)) {
			$phar[$entry]->setMetadata($meta);
		} else {
			$phar->setMetadata($meta);
		}
		$phar->stopBuffering();
	}
	// }}}
	// {{{ public function cli_cmd_inf_met_get
	/**
	 * Cli Command Inf Metaget
	 *
	 * @return string A description of the metaget arguments
	 */
	public function cli_cmd_inf_meta_get()
	{
		return "Get meta information of a PHAR entry or a PHAR package in serialized from. " .
			   "If no output file is specified for meta data then stdout is being used.\n" .
			   "You can also specify a particular index using -k. In that case the metadata is " .
			   "expected to be an array and the value of the given index is returned using echo " .
			   "rather than using serialize. If that index does not exist or no meta data is " .
			   "present then the return value is 1.";
	}
	// }}}
	// {{{ public function cli_cmd_arg_meta_get
	/**
	 * Cli Command arg metaget
	 *
	 * @return array  The arguments for meta get.
	 */
	public function cli_cmd_arg_meta_get()
	{
		return self::phar_args('Fek', 'phar');
	}
	// }}}
	// {{{ public function cli_cmd_run_meta_get
	/**
	 * Cli Command Run Metaget
	 *
	 * @see $this->args
	 * @see $phar[$x]->hasMetadata()
	 * @see $phar->getMetadata()
	 */
	public function cli_cmd_run_meta_get()
	{
		$phar  = $this->args['f']['val'];
		$entry = $this->args['e']['val'];
		$index = $this->args['k']['val'];

		if (isset($entry)) {
			if (!$phar[$entry]->hasMetadata()) {
				echo "No Metadata\n";
				exit(1);
			}
			echo serialize($phar[$entry]->getMetadata());
		} else {
			if (!$phar->hasMetadata()) {
				echo "No Metadata\n";
				exit(1);
			}
			$meta = $phar->getMetadata();
		}

		if (isset($index)) {
			if (isset($index)) {
				if (isset($meta[$index])) {
					echo $meta[$index];
					exit(0);
				} else {
					echo "No Metadata\n";
					exit(1);
				}
			} else {
				echo serialize($meta);
			}
		}
	}
	// }}}
	// {{{ public function cli_cmd_inf_meta_del
	/**
	 * Cli Command Inf Metadel
	 *
	 * @return string A description of the metadel function
	 */
	public function cli_cmd_inf_meta_del()
	{
		return "Delete meta information of a PHAR entry or a PHAR package.\n" .
			   "If -k is given then the metadata is expected to be an array " .
			   "and the given index is being deleted.\n" .
			   "If something was deleted the return value is 0 otherwise it is 1.";
	}
	// }}}
	// {{{ public function cli_cmd_arg_meta_del
	/**
	 * CliC ommand Arg Metadelete
	 *
	 * @return array The arguments for metadel
	 */
	public function cli_cmd_arg_meta_del()
	{
		return self::phar_args('Fek', 'phar');
	}
	// }}}
	// {{{ public function cli_cmd_run_meta_del
	/**
	 * Cli Command Run MetaDel
	 *
	 * @see $phar[$x]->delMetadata()
	 * @see $phar->delMetadata()
	 */
	public function cli_cmd_run_meta_del()
	{
		$phar  = $this->args['f']['val'];
		$entry = $this->args['e']['val'];
		$index = $this->args['k']['val'];

		if (isset($entry)) {
			if (isset($index)) {
				if (!$phar[$entry]->hasMetadata()) {
					exit(1);
				}
				$meta = $phar[$entry]->getMetadata();

				// @todo add error message here.
				if (!is_array($meta)) {
					exit(1);
				}

				unset($meta[$index]);
				$phar[$entry]->setMetadata($meta);
			} else {
				exit($phar[$entry]->delMetadata() ? 0 : 1);
			}
		} else {
			if (isset($index)) {
				if (!$phar->hasMetadata()) {
					exit(1);
				}

				$meta = $phar->getMetadata();

				// @todo Add error message
				if (!is_array($meta)) {
					exit(1);
				}

				unset($meta[$index]);
				$phar->setMetadata($meta);
			} else {
				exit($phar->delMetadata() ? 0 : 1);
			}
		}
	}
	// }}}
	// {{{ public function cli_cmd_inf_info
	/**
	 * CLi Command Inf Info
	 *
	 * @return string A description about the info commands.
	 */
	public function cli_cmd_inf_info()
	{
		return "Get information about a PHAR package.\n" .
			   "By using -k it is possible to return a single value.";
	}
	// }}}
	// {{{ public function cli_cmd_arg_info
	/**
	 * Cli Command Arg Infos
	 *
	 * @return array The arguments for info command.
	 */
	public function cli_cmd_arg_info()
	{
		return self::phar_args('Fk', 'phar');
	}
	// }}}
	// {{{ public function cli_cmd_run_info
	/**
	 * Cli Command Run Info
	 *
	 * @param args $args
	 */
	public function cli_cmd_run_info()
	{
		$phar  = $this->args['f']['val'];
		$index = $this->args['k']['val'];

		$hash  = $phar->getSignature();
		$infos = array();

		if ($phar->getAlias()) {
			$infos['Alias'] = $phar->getAlias();
		}

		if (!$hash) {
			$infos['Hash-type'] = 'NONE';
		} else {
			$infos['Hash-type'] = $hash['hash_type'];
			$infos['Hash'] = $hash['hash'];
		}

		$csize   = 0;
		$usize   = 0;
		$count   = 0;
		$ccount  = 0;
		$ucount  = 0;
		$mcount  = 0;
		$compalg = array('GZ'=>0, 'BZ2'=>0);

		foreach(new RecursiveIteratorIterator($phar) as $ent) {
			$count++;
			if ($ent->isCompressed()) {
				$ccount++;
				$csize += $ent->getCompressedSize();
				if ($ent->isCompressed(Phar::GZ)) {
					$compalg['GZ']++;
				} elseif ($ent->isCompressed(Phar::BZ2)) {
					$compalg['BZ2']++;
				}
			} else {
				$ucount++;
				$csize += $ent->getSize();
			}

			$usize += $ent->getSize();

			if ($ent->hasMetadata()) {
				$mcount++;
			}
		}

		$infos['Entries']            = $count;
		$infos['Uncompressed-files'] = $ucount;
		$infos['Compressed-files']   = $ccount;
		$infos['Compressed-gz']      = $compalg['GZ'];
		$infos['Compressed-bz2']     = $compalg['BZ2'];
		$infos['Uncompressed-size']  = $usize;
		$infos['Compressed-size']    = $csize;
		$infos['Compression-ratio']  = sprintf('%.3g%%', $usize ? ($csize * 100) / $usize : 100);
		$infos['Metadata-global']    = $phar->hasMetadata() * 1;
		$infos['Metadata-files']     = $mcount;
		$infos['Stub-size']          = strlen($phar->getStub());

		if (isset($index)) {
			if (!isset($infos[$index])) {
				self::error("Requested value does not exist.\n");
			}

			echo $infos[$index];
			exit(0);
		}

		$l = 0;
		foreach($infos as $which => $val) {
			$l = max(strlen($which), $l);
		}

		foreach($infos as $which => $val) {
			echo $which . ':' . str_repeat(' ', $l + 1 - strlen($which)) . $val . "\n";
		}
	}
	// }}}
	// {{{ public function cli_cmd_inf_version
	/**
	 * CLi Command Inf Version
	 *
	 * @return string A description about the info commands.
	 */
	public function cli_cmd_inf_version()
	{
		return "Get information about the PHAR environment and the tool version.";
	}
	// }}}
	// {{{ public function cli_cmd_arg_version
	/**
	 * Cli Command Arg Version
	 *
	 * @return array The arguments for version command.
	 */
	public function cli_cmd_arg_version()
	{
		return self::phar_args('', NULL);
	}
	// }}}
	// {{{ public function cli_cmd_run_info
	/**
	 * Cli Command Run Info
	 *
	 * @param args $args
	 */
	public function cli_cmd_run_version()
	{
		$use_ext = extension_loaded('phar');
		$version = array(
			'PHP Version' => phpversion(),
			'phar.phar version' => '$Id$',
			'Phar EXT version' => $use_ext ? phpversion('phar') : 'Not available',
			'Phar API version' => Phar::apiVersion(),
			'Phar-based phar archives' => true,
			'Tar-based phar archives' => $use_ext,
			'ZIP-based phar archives' => $use_ext,
			'gzip compression' => extension_loaded('zlib'),
			'bzip2 compression' => extension_loaded('bz2'),
			'supported signatures' => $use_ext ? join(', ', Phar::getSupportedSignatures()) : '',
			);
		$klen = 0;
		foreach($version as $k => $v)
		{
			$klen = max($klen, strlen($k));
		}
		++$klen;
		foreach($version as $k => $v) {
			if (is_bool($v)) {
				$v = $v ? 'enabled' : 'disabled';
			}
			printf("%-${klen}s  %s\n", $k.':', $v);
		}
	}
	// }}}
}
// }}}
?>
