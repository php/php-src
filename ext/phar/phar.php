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

if (!class_exists('CLICommand'))
{
	abstract class CLICommand
	{
		protected $argc;
		protected $argv;
		protected $cmds = array();
		protected $args = array();

		function __construct($argc, array $argv)
		{
			$this->argc = $argc;
			$this->argv = $argv;
			$this->cmds = self::getCommands($this);
			$this->types= self::getArgTypes($this);
			
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
								$type = $this->args[$arg]['type'];

								if (isset($this->types[$type]['typ']))
								{
									$this->args[$arg]['val'] = call_user_func(array($this, $this->types[$type]['typ']), $argv[$i]);
								}
								else
								{
									$this->args[$arg]['val'] = $argv[$i];
								}
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
			
			call_user_func(array($this, $this->cmds[$command]['run']));
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
		
		static function getArgTypes(CLICommand $cmdclass)
		{
			return self::getSubFuncs($cmdclass, 'cli_arg_', array('typ'));
		}
	}
}

class PharCommand extends CLICommand
{
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

	static function cli_cmd_inf_help()
	{
		return "This help.";
	}

	function cli_cmd_run_help()
	{
		$argv = $this->argv;

		echo <<<EOF
$argv[0] command [options]

General options:
-f <file>   Specifies the phar file to work on.
-a <alias>  Specifies the phar alias.
-r <regex>  Specifies a regular expression for input files.

Commands:

EOF;
		$l = 0;
		foreach($this->cmds as $name => $funcs)
		{
			$l = max($l, strlen($name));
		}
		foreach($this->cmds as $name => $funcs)
		{
			if (isset($funcs['inf']))
			{
				$inf = call_user_func(array($this, $funcs['inf']));
			}
			else
			{
				$inf = "";
			}
			printf("%${l}s  %s\n\n", $name, $inf);
		}
		exit(0);
	}

	static function cli_cmd_inf_pack()
	{
		return "Pack files into a PHAR archive.";
	}

	static function cli_cmd_arg_pack()
	{
		return array(
			'f' => array('type'=>'pharnew', 'val'=>NULL,      'required'=>1),
			'a' => array('type'=>'alias',   'val'=>'newphar', 'required'=>1),
			''  => array('type'=>'any',     'val'=>NULL,      'required'=>1),
			'r' => array('type'=>'regex',   'val'=>NULL),
			);
	}

	static function cli_cmd_run_pack()
	{
		$archive = $this->args['f'];
		$alias   = $this->args['a'];
		$input   = $this->args['i'];
		$regex   = $this->args['r'];

		$phar  = new Phar($archive, 0, $alias);
		$dir   = new RecursiveDirectoryIterator($input);
		$dir   = new RecursiveIteratorIterator($dir);
		if (isset($regex))
		{
			$dir  = new RegexIterator($dir, '/'. $argv[3].'/');
		}

		try
		{
			$phar->begin();
			foreach($dir as $f)
			{
				echo "$f\n";
				$phar[$file] = file_get_contents($file);
			}
			$phar->compressAllFilesBZIP2();
			$phar->commit();
			exit(0);
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
			'f' => array('type'=>'pharurl', 'val'=>NULL, 'required'=>1),
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
			'f' => array('type'=>'pharurl', 'val'=>NULL, 'required'=>1),
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

new PharCommand($argc, $argv);

?>