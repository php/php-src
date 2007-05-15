<?php

/** @file pharcommand.inc
 * @ingroup Phar
 * @brief class CLICommand
 * @author  Marcus Boerger
 * @date    2007 - 2007
 *
 * Phar Command
 */

/** @ingroup Phar
 * @brief   Phar console command implementation
 * @author  Marcus Boerger
 * @version 1.0
 */
class PharCommand extends CLICommand
{
	function cli_get_SP2($l1, $arg_inf)
	{
		return str_repeat(' ', $l1 + 2 + 4 + 9);
	}

	function cli_get_SP3($l1, $l2, $arg_inf)
	{
		return str_repeat(' ', $l1 + 2 + 4 + 9 + 2 + $l2 + 2);
	}

	static function phar_args($which, $phartyp)
	{
		$phar_args = array(
			'a' => array('typ'=>'alias',  'val'=>'newphar', 'inf'=>'<alias>  Provide an alias name for the phar file.'),
			'c' => array('typ'=>'compalg','val'=>NULL,      'inf'=>'<algo>   Compression algorithmus.', 'select'=>array('0'=>'No compression','none'=>'No compression','auto'=>'Automatically select compression algorithm')),
			'e' => array('typ'=>'entry',  'val'=>NULL,      'inf'=>'<entry>  Name of entry to work on (must include PHAR internal directory name if any).'),
			'f' => array('typ'=>$phartyp, 'val'=>NULL,      'inf'=>'<file>   Specifies the phar file to work on.'),
			'h' => array('typ'=>'select', 'val'=>NULL,      'inf'=>'<method> Selects the hash algorithmn.', 'select'=>array('md5'=>'MD5','sha1'=>'SHA1')),
			'i' => array('typ'=>'regex',  'val'=>NULL,      'inf'=>'<regex>  Specifies a regular expression for input files.'),
			'm' => array('typ'=>'any',    'val'=>NULL,      'inf'=>'<meta>   Meta data to store with entry (serialized php data).'),
			's' => array('typ'=>'file',   'val'=>NULL,      'inf'=>'<stub>   Select the stub file.'),
			'x' => array('typ'=>'regex',  'val'=>NULL,      'inf'=>'<regex>  Regular expression for input files to exclude.'),
		);
		if (extension_loaded('zlib'))
		{
			$phar_args['c']['select']['gz']    = 'GZip compression';
			$phar_args['c']['select']['gzip']  = 'GZip compression';
		}
		if (extension_loaded('bz2'))
		{
			$phar_args['c']['select']['bz2']   = 'BZip2 compression';
			$phar_args['c']['select']['bzip2'] = 'BZip2 compression';
		}
		$hash_avail = Phar::getSupportedSignatures();
		if (in_array('SHA-256', $hash_avail))
		{
			$phar_args['h']['select']['sha256'] = 'SHA256';
		}
		if (in_array('SHA-512', Phar::getSupportedSignatures()))
		{
			$phar_args['h']['select']['sha512'] = 'SHA512';
		}
		$args = array();
		foreach($phar_args as $lkey => $cfg)
		{
			$ukey = strtoupper($lkey);
			$required = strpos($which, $ukey) !== false;
			$optional = strpos($which, $lkey) !== false;
			if ($required || $optional)
			{
				$args[$lkey] = $cfg;
				$args[$lkey]['required'] = $required;
			}
		}
		return $args;
	}

	static function strEndsWith($haystack, $needle)
	{
		return substr($haystack, -strlen($needle)) == $needle;
	}

	static function cli_arg_typ_pharnew($arg)
	{
		$arg = self::cli_arg_typ_filenew($arg);
		if (!Phar::isValidPharFilename($arg))
		{
			echo "Phar files must have file extension '.phar', '.phar.php', '.phar.bz2' or 'phar.gz'.\n";
			exit(1);
		}
		return $arg;
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

	static function cli_arg_typ_entry($arg)
	{
		// no further check atm, maybe check for no '/' at beginning
		return $arg;
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
		return "Pack files into a PHAR archive.\n"
			.  "When using -s <stub>, then the stub file is being excluded from the list of input files/dirs."
			;
	}

	static function cli_cmd_arg_pack()
	{
		$args = self::phar_args('AcFhisx', 'pharnew');
		$args[''] = array('typ'=>'any',     'val'=>NULL,      'required'=>1, 'inf'=>'         Any number of input files and directories. If -i is in use then ONLY files and matching thegiven regular expression are being packed. If -x is given then files matching that regular expression are NOT being packed.');
		return $args;
	}

	function cli_cmd_run_pack()
	{
		if (ini_get('phar.readonly'))
		{
			echo "Creating phar files is disabled by ini setting 'phar.readonly'.\n";
			exit(1);
		}
		if (Phar::canWrite())
		{
			echo "Creating phar files is disabled, Phar::canWrite() returned false.\n";
			exit(1);
		}

		$alias   = $this->args['a']['val'];
		$archive = $this->args['f']['val'];
		$hash    = $this->args['h']['val'];
		$regex   = $this->args['i']['val'];
		$stub    = $this->args['s']['val'];
		$invregex= $this->args['x']['val'];
		$input   = $this->args['']['val'];

		$phar  = new Phar($archive, 0, $alias);

		$phar->startBuffering();

		if (isset($stub))
		{
			$phar->setStub(file_get_contents($stub));
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
			$phar->uncompressAllFiles();
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
			$dir = new RegexIterator($dir, $regex);
		}

		if (isset($invregex))
		{
			$dir = new InvertedRegexIterator($dir, $invregex);
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

	function phar_dir_echo($pn, $f)
	{
		echo "$f\n";
	}

	function phar_dir_operation(RecursiveIteratorIterator $dir, $func, array $args = array())
	{
		$regex   = $this->args['i']['val'];
		$invregex= $this->args['x']['val'];

		if (isset($regex))
		{
			$dir = new RegexIterator($dir, $regex);
		}

		if (isset($invregex))
		{
			$dir = new InvertedRegexIterator($dir, $invregex);
		}

		foreach($dir as $pn => $f)
		{
			call_user_func($func, $pn, $f, $args);
		}
	}

	static function cli_cmd_inf_list()
	{
		return "List contents of a PHAR archive.";
	}

	static function cli_cmd_arg_list()
	{
		return self::phar_args('Fix', 'pharurl');
	}

	function cli_cmd_run_list()
	{
		$this->phar_dir_operation(new DirectoryTreeIterator($this->args['f']['val']), array($this, 'phar_dir_echo'));
	}

	static function cli_cmd_inf_tree()
	{
		return "Get a directory tree for a PHAR archive.";
	}

	static function cli_cmd_arg_tree()
	{
		return self::phar_args('Fix', 'pharurl');
	}

	function cli_cmd_run_tree()
	{
		$this->phar_dir_operation(new DirectoryGraphIterator($this->args['f']['val']), array($this, 'phar_dir_echo'));
	}

	static function cli_cmd_inf_extract()
	{
		return "Extract a PHAR package to a directory.";
	}

	static function cli_cmd_arg_extract()
	{
		$args = self::phar_args('Fix', 'phar');
		$args[''] = array('typ'=>'dir',  'val'=>'.',                 'inf'=>'         Directory to extract to (defaults to \'.\').');
		return $args;
	}

	function cli_cmd_run_extract()
	{
		$dir = $this->args['']['val'];
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
		
		$this->phar_dir_operation(new RecursiveIteratorIterator($phar), array($this, 'phar_dir_extract'), array($blen, $dir));
	}
	
	function phar_dir_extract($pn, $f, $args)
	{
		$blen   = $args[0];
		$dir    = $args[1];
		$sub    = substr($pn, $blen);
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

	function cli_cmd_inf_stub_set()
	{
		return "Set the stub of a PHAR file. "
		. "If no input file is specified as stub then stdin is being used.";
	}
	
	function cli_cmd_arg_stub_set()
	{
		$args = self::phar_args('Fs', 'phar');
		$args['s']['val'] = 'php://stdin';
		return $args;
	}
	
	function cli_cmd_run_stub_set($args)
	{
		$phar = $this->args['f']['val'];
		$stub = $this->args['s']['val'];
	
		$phar->setStub(file_get_contents($stub));
	}
	
	function cli_cmd_inf_stub_get()
	{
		return "Get the stub of a PHAR file. "
		. "If no output file is specified as stub then stdout is being used.";
	}

	function cli_cmd_arg_stub_get()
	{
		$args = self::phar_args('Fs', 'phar');
		$args['s']['val'] = 'php://stdin';
		return $args;
	}

	function cli_cmd_run_stub_get($args)
	{
		$phar = $this->args['f']['val'];
		$stub = $this->args['s']['val'];

		file_put_contents($stub, $phar->getStub());
	}

	function cli_cmd_inf_compress()
	{
		return "Compress or uncompress all files or a selected entry.";
	}

	function cli_cmd_arg_compress()
	{
		return self::phar_args('FCe', 'phar');
	}

	function cli_cmd_run_compress()
	{
		$phar  = $this->args['f']['val'];
		$entry = $this->args['e']['val'];

		switch($this->args['c']['val'])
		{
		case 'gz':
		case 'gzip':
			if (isset($entry))
			{
				$phar[$entry]->setCompressedGZ();
			}
			else
			{
				$phar->compressAllFilesGZ();
			}
			break;
		case 'bz2':
		case 'bzip2':
			if (isset($entry))
			{
				$phar[$entry]->setCompressedBZIP2();
			}
			else
			{
				$phar->compressAllFilesBZIP2();
			}
			break;
		default:
			if (isset($entry))
			{
				$phar[$entry]->setUncompressed();
			}
			else
			{
				$phar->uncompressAllFiles();
			}
			break;
		}
	}

	function cli_cmd_inf_sign()
	{
		return "Set signature hash algorithm.";
	}

	function cli_cmd_arg_sign()
	{
		return self::phar_args('FH', 'phar');
	}

	function cli_cmd_run_sign()
	{
		$phar = $this->args['f']['val'];
		$hash = $this->args['h']['val'];
		
		$phar->setSignatureAlgorithm($hash);
	}

	function cli_cmd_inf_meta_set()
	{
		return "Set meta data of a PHAR entry or a PHAR package using serialized input. "
		. "If no input file is specified for meta data then stdin is being used.";
	}

	function cli_cmd_arg_meta_set()
	{
		$args = self::phar_args('Fem', 'phar');
		$args['m']['val'] = 'php://stdin';
		return $args;
	}

	function cli_cmd_run_meta_set()
	{
		$phar  = $this->args['f']['val'];
		$entry = $this->args['e']['val'];
		$meta  = $this->args['m']['val'];

		$phar->startBuffering();
		if (isset($entry))
		{
			$phar[$entry]->setMetadata(unserialize($meta));
		}
		else
		{
			$phar->setMetadata(unserialize($meta));
		}
		$phar->stopBuffering();
	}

	function cli_cmd_inf_meta_get()
	{
		return "Get meta information of a PHAR entry or a PHAR package in serialized from. "
		. "If no output file is specified for meta data then stdout is being used.";
	}

	function cli_cmd_arg_meta_get()
	{
		$args = self::phar_args('Fem', 'phar');
		//$args['m']['val'] = 'php://stdout';
		return $args;
	}

	function cli_cmd_run_meta_get()
	{
		$phar  = $this->args['f']['val'];
		$entry = $this->args['e']['val'];

		if (isset($entry))
		{
			if (!$phar[$entry]->hasMetadata())
			{
				exit(1);
			}
			echo serialize($phar[$entry]->getMetadata());
		}
		else
		{
			if (!$phar->hasMetadata())
			{
				exit(1);
			}
			echo serialize($phar->getMetadata());
		}
	}

	function cli_cmd_inf_meta_del()
	{
		return "Delete meta information of a PHAR entry or a PHAR package.";
	}

	function cli_cmd_arg_meta_del()
	{
		return self::phar_args('Fe', 'phar');
	}

	function cli_cmd_run_meta_del()
	{
		$phar  = $this->args['f']['val'];
		$entry = $this->args['e']['val'];

		if (isset($entry))
		{
			exit($phar[$entry]->delMetadata() ? 0 : 1);
		}
		else
		{
			exit($phar->delMetadata() ? 0 : 1);
		}
	}

	function cli_cmd_inf_info()
	{
		return "Get information about a PHAR package.";
	}

	function cli_cmd_arg_info()
	{
		return self::phar_args('F', 'phar');		
	}

	function cli_cmd_run_info($args)
	{
		$phar  = $this->args['f']['val'];

		$hash  = $phar->getsignature();
		$infos = array();

		if (!$hash)
		{
			$infos['Hash-type'] = 'NONE';
		}
		else
		{
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

		foreach(new RecursiveIteratorIterator($phar) as $ent)
		{
			$count++;
			if ($ent->isCompressed())
			{
				$ccount++;
				$csize += $ent->getCompressedSize();
				if ($ent->isCompressedGZ())
				{
					$compalg['GZ']++;
				}
				else if ($ent->isCompressedBZIP2())
				{
					$compalg['BZ2']++;
				}
			}
			else
			{
				$ucount++;
				$csize += $ent->getSize();
			}
			$usize += $ent->getSize();
			if ($ent->hasMetadata())
			{
				$mcount++;
			}
		}

		$infos['Entries'] = $count;
		$infos['Uncompressed-files'] = $ucount;
		$infos['Compressed-files'] = $ccount;
		$infos['Compressed-gz'] = $compalg['GZ'];
		$infos['Compressed-bz2'] = $compalg['BZ2'];
		$infos['Uncompressed-size'] = $usize;
		$infos['Compressed-size'] = $csize;
		$infos['Compression-ratio'] = sprintf('%.3g%%', ($csize * 100) / $usize);
		$infos['Metadata'] = $mcount;

		$l = 0;
		foreach($infos as $which => $val)
		{
			$l = max(strlen($which), $l);
		}
		foreach($infos as $which => $val)
		{
			echo $which . ':' . str_repeat(' ', $l + 1 - strlen($which)) . $val . "\n";
		}
	}
}

?>