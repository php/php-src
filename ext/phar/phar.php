#!/usr/bin/php
<?php

foreach(array("SPL", "Reflection", "Phar") as $ext)
{
	if (!extension_loaded($ext))
	{
		echo "$argv[0] requires PHP extension $ext.\n";
		exit(1);
	}
}

if (!class_exists('DirectoryTreeIterator'))
{
	class DirectoryTreeIterator extends RecursiveIteratorIterator
	{
		function __construct($path)
		{
			parent::__construct(
				new RecursiveCachingIterator(
					new RecursiveDirectoryIterator($path, RecursiveDirectoryIterator::KEY_AS_FILENAME
					),
					CachingIterator::CALL_TOSTRING|CachingIterator::CATCH_GET_CHILD
				),
				parent::SELF_FIRST
			);
		}

		function current()
		{
			$tree = '';
			for ($l=0; $l < $this->getDepth(); $l++) {
				$tree .= $this->getSubIterator($l)->hasNext() ? '| ' : '  ';
			}
			return $tree . ($this->getSubIterator($l)->hasNext() ? '|-' : '\-')
			       . $this->getSubIterator($l)->__toString();
		}

		function __call($func, $params)
		{
			return call_user_func_array(array($this->getSubIterator(), $func), $params);
		}
	}
}

if (!class_exists('DirectoryGraphIterator'))
{
	class DirectoryGraphIterator extends DirectoryTreeIterator
	{
		function __construct($path)
		{
			RecursiveIteratorIterator::__construct(
				new RecursiveCachingIterator(
					new ParentIterator(
						new RecursiveDirectoryIterator($path, RecursiveDirectoryIterator::KEY_AS_FILENAME
						)
					),
					CachingIterator::CALL_TOSTRING|CachingIterator::CATCH_GET_CHILD
				),
				parent::SELF_FIRST
			);
		}
	}
}

if (!class_exists('InvertedRegexIterator'))
{
	class InvertedRegexIterator extends RegexIterator
	{
		function accept()
		{
			return !RegexIterator::accept();
		}
	}
}

if (!class_exists('CLICommand'))
{
	abstract class CLICommand
	{
		protected $argc;
		protected $argv;
		protected $cmds = array();
		protected $args = array();
		protected $typs = array();

		function __construct($argc, array $argv)
		{
			$this->argc = $argc;
			$this->argv = $argv;
			$this->cmds = self::getCommands($this);
			$this->typs = self::getArgTyps($this);

			if ($argc < 2)
			{
				echo "No command given, check ${argv[0]} help\n";
				exit(1);
			}
			elseif (!isset($this->cmds[$argv[1]]['run']))
			{
				echo "Unknown command '${argv[1]}', check ${argv[0]} help\n";
				exit(1);
			}
			else
			{
				$command = $argv[1];
			}

			if (isset($this->cmds[$command]['arg']))
			{
				$this->args = call_user_func(array($this, $this->cmds[$command]['arg']));
				$i = 1;
				$missing = false;
				while (++$i < $argc)
				{
					if ($argv[$i][0] == '-')
					{
						if (strlen($argv[$i]) == 2 && isset($this->args[$argv[$i][1]]))
						{
							$arg = $argv[$i][1];
							if (++$i >= $argc)
							{
								echo "Missing argument to parameter '$arg' of command '$command', check ${argv[0]} help\n";
								exit(1);
							}
							else
							{
								$this->args[$arg]['val'] = $this->checkArgTyp($arg, $i, $argc, $argv);
							}
						}
						else
						{
							echo "Unknown parameter '${argv[$i]}' to command $command, check ${argv[0]} help\n";
							exit(1);
						}
					}
					else
					{
						break;
					}
				}
				if (isset($this->args['']))
				{
					if ($i >= $argc)
					{
						if (isset($this->args['']['require']) && $this->args['']['require'])
						{
							echo "Missing default trailing arguments to command $command, check ${argv[0]} help\n";
							$missing = true;
						}
					}
					else
					{
						$this->args['']['val'] = array();
						while($i < $argc)
						{
							$this->args['']['val'][] = $argv[$i++];
						}
					}
				}
				else if ($i < $argc)
				{
					echo "Unexpected default arguments to command $command, check ${argv[0]} help\n";
					exit(1);
				}
				foreach($this->args as $arg => $inf)
				{
					if (strlen($arg) && !isset($inf['val']) && isset($inf['required']) && $inf['required'])
					{
						echo "Missing parameter '-$arg' to command $command, check ${argv[0]} help\n";
						$missing = true;
					}
				}
				if ($missing)
				{
					exit(1);
				}
			}

			call_user_func(array($this, $this->cmds[$command]['run']), $this->args);
		}

		function checkArgTyp($arg, $i, $argc, $argv)
		{
			$typ = $this->args[$arg]['typ'];

			if (isset($this->typs[$typ]['typ']))
			{
				return call_user_func(array($this, $this->typs[$typ]['typ']), $argv[$i], $this->args[$arg]);
			}
			else
			{
				return $argv[$i];
			}
		}

		static function getSubFuncs(CLICommand $cmdclass, $prefix, array $subs)
		{
			$a = array();
			$r = new ReflectionClass($cmdclass);
			$l = strlen($prefix);

			foreach($r->getMethods() as $m)
			{
				if (substr($m->name, 0, $l) == $prefix)
				{
					foreach($subs as $sub)
					{
						$what = substr($m->name, $l+strlen($sub)+1);
						$func = $prefix . $sub . '_' . $what;
						if ($r->hasMethod($func))
						{
							if (!isset($a[$what]))
							{
								$a[$what] = array();
							}
							$a[$what][$sub] = /*$m->class . '::' .*/ $func;
						}
					}
				}
			}
			return $a;
		}

		static function getCommands(CLICommand $cmdclass)
		{
			return self::getSubFuncs($cmdclass, 'cli_cmd_', array('arg','inf','run'));
		}

		static function getArgTyps(CLICommand $cmdclass)
		{
			return self::getSubFuncs($cmdclass, 'cli_arg_', array('typ'));
		}

		static function cli_arg_typ_bool($arg)
		{
			return (bool)$arg;
		}

		static function cli_arg_typ_select($arg, $cfg)
		{
			if (!in_array($arg, array_keys($cfg['select'])))
			{
				echo "Parameter value '$arg' not one of '" . join("', '", array_keys($cfg['select'])) . "'.\n";
				exit(1);
			}
			return $arg;
		}

		static function cli_arg_typ_dir($arg)
		{
			$f = realpath($arg);
			if ($f===false || !file_exists($f) || !is_dir($f))
			{
				echo "Requested path '$arg' does not exist.\n";
				exit(1);
			}
			return $f;
		}

		static function cli_arg_typ_file($arg)
		{
			$f = realpath($arg);
			if ($f===false || !file_exists($f))
			{
				echo "Requested file '$arg' does not exist.\n";
				exit(1);
			}
			return $f;
		}

		static function cli_arg_typ_filenew($arg)
		{
			$d = dirname($arg);
			$f = realpath($d);
			if ($f === false)
			{
				echo "Path for file '$arg' does not exist.\n";
				exit(1);
			}
			return $f . substr($arg, strlen($d));;
		}

		static function cli_arg_typ_filecont($arg)
		{
			return file_get_contents(self::cli_arg_typ_file($arg));
		}

		function cli_get_SP2($l1, $l2, $arg_inf)
		{
			return str_repeat(' ', $l1 + 2 + 8);
		}

		static function cli_cmd_inf_help()
		{
			return "This help.";
		}

		function cli_cmd_run_help()
		{
			$argv = $this->argv;

			echo <<<EOF
$argv[0] <command> [options]

Commands:


EOF;
			$l = 0;
			ksort($this->cmds);
			foreach($this->cmds as $name => $funcs)
			{
				$l = max($l, strlen($name));
			}
			$sp = str_repeat(' ', $l+2);
			foreach($this->cmds as $name => $funcs)
			{
				$inf = $name . substr($sp, strlen($name));
				if (isset($funcs['inf']))
				{
					$inf .= call_user_func(array($this, $funcs['inf'])) . "\n";
					if (isset($funcs['arg']))
					{
						$inf .= "\n";
						foreach(call_user_func(array($this, $funcs['arg'])) as $arg => $conf)
						{
							if (strlen($arg))
							{
								$arg = "-$arg  ";
							}
							else
							{
								$arg = "... ";
							}
							$inf .= $sp . $arg . $conf['inf'] . "\n";
							if ($conf['typ'] == 'select')
							{
								$l2  = 0;
								foreach($conf['select'] as $opt => $what)
								{
									$l2 = max($l2, strlen($opt));
								}
								$sp2 = $this->cli_get_SP2($l, $l2, $inf);
								foreach($conf['select'] as $opt => $what)
								{
									$inf .= $sp2 . sprintf("%-${l2}s  ", $opt) . $what . "\n";
								}
							}
						}
					}
				}
				echo "$inf\n\n";
			}
			exit(0);
		}
	}
}

class PharCommand extends CLICommand
{
	function cli_get_SP2($l1, $l2, $arg_inf)
	{
		return str_repeat(' ', $l1 + 2 + 17);
	}

	static function strEndsWith($haystack, $needle)
	{
		return substr($haystack, -strlen($needle)) == $needle;
	}

	static function cli_arg_typ_loader($arg)
	{
		if ($arg == 'auto')
		{
			$found = NULL;
			foreach(split("\n", `pear list-files PHP_Archive`) as $ent)
			{
				$matches = NULL;
				if (preg_match(",^php[ \t]+([^ \t].*pear[\\\\/]PHP[\\\\/]Archive.php)$,", $ent, $matches))
				{
					$found = $matches[1];
					break;
				}
			}
			if (!isset($found) && $arg != 'optional') {
				echo "Pear package PHP_Archive or Archive.php class file not found.\n";
				exit(1);
			}
			$arg = $found;
		}
		return self::cli_arg_typ_file($arg);
	}

	static function cli_arg_typ_pharnew($arg)
	{
		$pharfile = self::cli_arg_typ_filenew($arg);
		if (!self::strEndsWith($pharfile, '.phar')
		&&  !self::strEndsWith($pharfile, '.phar.php')
		&&  !self::strEndsWith($pharfile, '.phar.bz2')
		&&  !self::strEndsWith($pharfile, '.phar.gz')
		)
		{
			echo "Phar files must have file extension '.pahr', '.parh.php', '.phar.bz2' or 'phar.gz'.\n";
			exit(1);
		}
		return $pharfile;
	}

	static function cli_arg_typ_pharfile($arg)
	{
		try
		{
			$pharfile = self::cli_arg_typ_file($arg);
			if (!Phar::loadPhar($pharfile))
			{
				"Unable to open phar '$arg'\n";
				exit(1);
			}
			return $pharfile;
		}
		catch(Exception $e)
		{
			echo "Exception while opening phar '$arg':\n";
			echo $e->getMessage() . "\n";
			exit(1);
		}
	}

	static function cli_arg_typ_pharurl($arg)
	{
		return 'phar://' . self::cli_arg_typ_pharfile($arg);
	}

	static function cli_arg_typ_phar($arg)
	{
		try
		{
			return new Phar(self::cli_arg_typ_pharfile($arg));
		}
		catch(Exception $e)
		{
			echo "Exception while opening phar '$argv':\n";
			echo $e->getMessage() . "\n";
			exit(1);
		}
	}

	static function cli_arg_typ_compalg($arg, $cfg)
	{
		$arg = self::cli_arg_typ_select($arg, $cfg);
		switch($arg)
		{
		case 'auto':
			if (extension_loaded('zlib'))
			{
				$arg = 'gz';
			}
			else if (extension_loaded('bz2'))
			{
				$arg = 'bz2';
			}
			else
			{
				$arg = '0';
			}
			break;
		}
		return $arg;
	}

	static function cli_cmd_inf_pack()
	{
		return "Pack files into a PHAR archive.";
	}

	static function cli_cmd_arg_pack()
	{
		return array(
			'a' => array('typ'=>'alias',   'val'=>'newphar', 'required'=>1, 'inf'=>'<alias>  Provide an alias name for the phar file.'),
			'c' => array('typ'=>'compalg', 'val'=>NULL,                     'inf'=>'<algo>   Compression algorithmus.', 'select'=>array('gz'=>'GZip compression','gzip'=>'GZip compression','bzip2'=>'BZip2 compression','bz'=>'BZip2 compression','bz2'=>'BZip2 compression','0'=>'No compression','none'=>'No compression','auto'=>'Automatically select compression algorithm')),
			'f' => array('typ'=>'pharnew', 'val'=>NULL,      'required'=>1, 'inf'=>'<file>   Specifies the phar file to work on.'),
			'h' => array('typ'=>'select',  'val'=>NULL,                     'inf'=>'<method> Selects the hash algorithmn.', 'select'=>array('md5'=>'MD5','sha1'=>'SHA1','sha256'=>'SHA256','sha512'=>'SHA512')),
			'i' => array('typ'=>'regex',   'val'=>NULL,                     'inf'=>'<regex>  Specifies a regular expression for input files.'),
			'p' => array('typ'=>'loader',  'val'=>NULL,                     'inf'=>'<loader> Location of PHP_Archive class file (pear list-files PHP_Archive). You can use \'auto\' or \'optional\' to locate it automatically using the mentioned pear command. When using \'optional\' the command does not error out when the class file cannot be located.'),
			's' => array('typ'=>'file',    'val'=>NULL,                     'inf'=>'<stub>   Select the stub file (excluded from list of input files/dirs).'),
			'x' => array('typ'=>'regex',   'val'=>NULL,                     'inf'=>'<regex>  Regular expression for input files to exclude.'),
			''  => array('typ'=>'any',     'val'=>NULL,      'required'=>1, 'inf'=>'         Any number of input files and directories.'),
			);
	}

	function cli_cmd_run_pack()
	{
		if (ini_get('phar.readonly'))
		{
			echo "Creating phar files is disabled by ini setting 'phar.readonly'.\n";
			exit(1);
		}
		if (Phar::canWrite() == version_compare(phpversion('phar'), '1.2', '<'))
		{
			echo "Creating phar files is disabled, Phar::canWrite() returned false.\n";
			exit(1);
		}

		$alias   = $this->args['a']['val'];
		$archive = $this->args['f']['val'];
		$hash    = $this->args['h']['val'];
		$regex   = $this->args['i']['val'];
		$loader  = $this->args['p']['val'];
		$stub    = $this->args['s']['val'];
		$invregex= $this->args['x']['val'];
		$input   = $this->args['']['val'];

		$phar  = new Phar($archive, 0, $alias);

		$phar->startBuffering();

		if (isset($stub))
		{
			if (isset($loader))
			{
				$c = file_get_contents($stub);
				$s = '';
				if (substr($c,0,2) == '#!') {
					$s.= substr($c,0,strpos($c, "\n")+1);
				}				
				$s.= '<?php if (!class_exists("Phar",0) && !class_exists("PHP_Archive")) { ?>';
				$s.= file_get_contents($loader);
				$s.= '<?php class Phar extends PHP_Archive {} } ?>';
				if (substr($c,0,1) == '#') {
					$s.= substr($c,strpos($c, "\n")+1);
				}
				$phar->setStub($s);
			}
			else
			{
				$phar->setStub(file_get_contents($stub));
			}
			$stub = new SplFileInfo($stub);
		}

		if (!is_array($input))
		{
			$this->phar_add($phar, $input, $regex, $invregex, $stub);
		}
		else
		{
			foreach($input as $i)
			{
				$this->phar_add($phar, $i, $regex, $invregex, $stub);
			}
		}

		switch($this->args['c']['val'])
		{
		case 'gz':
		case 'gzip':
			$phar->compressAllFilesGZ();
			break;
		case 'bz2':
		case 'bzip2':
			$phar->compressAllFilesBZIP2();
			break;
		default:
			break;
		}

		if ($hash)
		{		
			$phar->setSignatureAlgorithm($hash);
		}

		$phar->stopBuffering();
		exit(0);
	}

	static function phar_add(Phar $phar, $input, $regex, $invregex, SplFileInfo $stub)
	{
		$dir   = new RecursiveDirectoryIterator($input);
		$dir   = new RecursiveIteratorIterator($dir);

		if (isset($regex))
		{
			$dir = new RegexIterator($dir, '/'. $regex . '/');
		}

		if (isset($invregex))
		{
			$dir = new InvertedRegexIterator($dir, '/'. $invregex . '/');
		}

		try
		{
			foreach($dir as $file)
			{
				if (empty($stub) || $file->getRealPath() != $stub->getRealPath())
				{
					$f = $dir->getSubPathName();
					echo "$f\n";
					$phar[$f] = file_get_contents($file);
				}
			}
		}
		catch(Excpetion $e)
		{
			echo "Unable to complete operation on file '$file'\n";
			echo $e->getMessage() . "\n";
			exit(1);
		}
	}

	static function cli_cmd_inf_list()
	{
		return "List contents of a PHAR archive.";
	}

	static function cli_cmd_arg_list()
	{
		return array(
			'f' => array('typ'=>'pharurl', 'val'=>NULL, 'required'=>1, 'inf'=>'<file>   Specifies the phar file to work on.'),
			);
	}

	function cli_cmd_run_list()
	{
		foreach(new DirectoryTreeIterator($this->args['f']['val']) as $f)
		{
			echo "$f\n";
		}
	}

	static function cli_cmd_inf_tree()
	{
		return "Get a directory tree for a PHAR archive.";
	}

	static function cli_cmd_arg_tree()
	{
		return array(
			'f' => array('typ'=>'pharurl', 'val'=>NULL, 'required'=>1, 'inf'=>'<file>   Specifies the phar file to work on.'),
			);
	}

	function cli_cmd_run_tree()
	{
		foreach(new DirectoryGraphIterator($this->args['f']['val']) as $f)
		{
			echo "$f\n";
		}
	}

	static function cli_cmd_inf_extract()
	{
		return "Extract a PHAR package to a directory.";
	}

	static function cli_cmd_arg_extract()
	{
		return array(
			'f' => array('typ'=>'phar', 'val'=>NULL, 'required'=>1, 'inf'=>'<file>   Specifies the PHAR file to extract.'),
			''  => array('typ'=>'dir',  'val'=>'.',                 'inf'=>'         Directory to extract to (defaults to \'.\').'),
			);
	}

	static function cli_cmd_run_extract($args)
	{
		$dir = $args['']['val'];
		if (is_array($dir))
		{
			if (count($dir) != 1)
			{
				echo "Only one target directory allowed.\n";
				exit(1);
			}
			else
			{
				$dir = $dir[0];
			}
		}
		$phar = $args['f']['val'];
		$base = $phar->getPathname();
		$bend = strpos($base, '.phar');
		$bend = strpos($base, '/', $bend);
		$base = substr($base, 0, $bend + 1);
		$blen = strlen($base);
		foreach(new RecursiveIteratorIterator($phar) as $pn => $f)
		{
			$sub = substr($pn, $blen);
			$target = $dir . '/' . $sub;
			if (!file_exists(dirname($target)))
			{
				if (!@mkdir(dirname($target)))
				{
					echo "  ..unable to create dir\n";
					exit(1);
				}
			}
			echo "$sub";
			if (!@copy($f, $target))
			{
				echo " ...error\n";
			}
			else
			{
				echo " ...ok\n";
			}
		}
	}
}

new PharCommand($argc, $argv);

?>
