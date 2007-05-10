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

	static function cli_arg_typ_pharfile($arg)
	{
		try
		{
			if (!file_exists($arg) && file_exists($ps = dirname(__FILE__).'/'.$arg))
			{
				$arg = $ps;
			}
			if (!Phar::loadPhar($arg))
			{
				"Unable to open phar '$phar'\n";
				exit(1);
			}
			return $arg;
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
			'f' => array('type'=>'pharnew', 'val'=>NULL,      'required'=>1, 'inf'=>'<file>   Specifies the phar file to work on.'),
			'a' => array('type'=>'alias',   'val'=>'newphar', 'required'=>1, 'inf'=>'<alias>  Provide an alias name for the phar file.'),
			's' => array('type'=>'file',    'val'=>NULL,                     'inf'=>'<stub>   Select the stub file (excluded from list of input files/dirs).'),
			'r' => array('type'=>'regex',   'val'=>NULL,                     'inf'=>'<regex>  Specifies a regular expression for input files.'),
			'c' => array('type'=>'select',  'val'=>NULL,                     'inf'=>'<algo>   Compression algorithmus.', 'select'=>array('gz'=>'GZip compression','gzip'=>'GZip compression','bzip2'=>'BZip2 compression','bz'=>'BZip2 compression','bz2'=>'BZip2 compression','0'=>'No compression','none'=>'No compression')),
			''  => array('type'=>'any',     'val'=>NULL,      'required'=>1, 'inf'=>'         Any number of input files and directories.'),
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
			$this->phar_add($phar, $input, $regex, $stub);
		}
		else
		{
			foreach($input as $i)
			{
				$this->phar_add($phar, $i, $regex, $stub);
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

	static function phar_add(Phar $phar, $input, $regex, SplFileInfo $stub)
	{
		$dir   = new RecursiveDirectoryIterator($input);
		$dir   = new RecursiveIteratorIterator($dir);

		if (isset($regex))
		{
			$dir = new RegexIterator($dir, '/'. $regex . '/');
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
			'f' => array('type'=>'pharurl', 'val'=>NULL, 'required'=>1, 'inf'=>'<file>   Specifies the phar file to work on.'),
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
			'f' => array('type'=>'pharurl', 'val'=>NULL, 'required'=>1, 'inf'=>'<file>   Specifies the phar file to work on.'),
			);
	}

	function cli_cmd_run_tree()
	{
		foreach(new DirectoryGraphIterator($this->args['f']['val']) as $f)
		{
			echo "$f\n";
		}
	}
}

?>