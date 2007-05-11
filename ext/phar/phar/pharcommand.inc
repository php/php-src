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
	function cli_get_SP2($l1, $l2, $arg_inf)
	{
		return str_repeat(' ', $l1 + 2 + 17);
	}

	static function strEndsWith($haystack, $needle)
	{
		return substr($haystack, -strlen($needle)) == $needle;
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

	static function cli_cmd_inf_pack()
	{
		return "Pack files into a PHAR archive.";
	}

	static function cli_cmd_arg_pack()
	{
		return array(
			'f' => array('typ'=>'pharnew', 'val'=>NULL,      'required'=>1, 'inf'=>'<file>   Specifies the phar file to work on.'),
			'a' => array('typ'=>'alias',   'val'=>'newphar', 'required'=>1, 'inf'=>'<alias>  Provide an alias name for the phar file.'),
			's' => array('typ'=>'file',    'val'=>NULL,                     'inf'=>'<stub>   Select the stub file (excluded from list of input files/dirs).'),
			'r' => array('typ'=>'regex',   'val'=>NULL,                     'inf'=>'<regex>  Specifies a regular expression for input files.'),
			'x' => array('typ'=>'regex',   'val'=>NULL,                     'inf'=>'<regex>  Regular expression for input files to exclude.'),
			'c' => array('typ'=>'select',  'val'=>NULL,                     'inf'=>'<algo>   Compression algorithmus.', 'select'=>array('gz'=>'GZip compression','gzip'=>'GZip compression','bzip2'=>'BZip2 compression','bz'=>'BZip2 compression','bz2'=>'BZip2 compression','0'=>'No compression','none'=>'No compression')),
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

		$archive = $this->args['f']['val'];
		$alias   = $this->args['a']['val'];
		$stub    = $this->args['s']['val'];
		$regex   = $this->args['r']['val'];
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

		$phar->stopBuffering();

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

?>