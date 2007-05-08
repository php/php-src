#!/usr/bin/php
<?php

$cmds = array();

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

class PharCommand
{
	static function phar_inf_help()
	{
		return "This help.";
	}

	static function phar_cmd_help()
	{
		global $cmds, $argc, $argv;

		echo <<<EOF
$argv[0] command [options]

General options:
-f <file>   Specifies the phar file to work on.
-a <alias>  Specifies the phar alias.
-r <regex>  Specifies a regular expression for input files.

Commands:

EOF;
		$l = 0;
		foreach($cmds as $name => $funcs)
		{
			$l = max($l, strlen($name));
		}
		foreach($cmds as $name => $funcs)
		{
			if (isset($funcs['inf']))
			{
				$inf = call_user_func($funcs['inf']);
			}
			else
			{
				$inf = "";
			}
			printf("%${l}s  %s\n\n", $name, $inf);
		}
		exit(0);
	}

	static function phar_inf_pack()
	{
		return "Pack files into a PHAR archive.";
	}

	static function phar_arg_pack()
	{
		return array(
			'f' => array('type'=>'pharnew', 'val'=>NULL,      'required'=>1),
			'a' => array('type'=>'alias',   'val'=>'newphar', 'required'=>1),
			''  => array('type'=>'any',     'val'=>NULL,      'required'=>1),
			'r' => array('type'=>'regex',   'val'=>NULL),
			);
	}

	static function phar_cmd_pack()
	{
		global $args;

		$archive = $args['f'];
		$alias   = $args['a'];
		$input   = $args['i'];
		$regex   = $args['r'];

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

	static function phar_inf_list()
	{
		return "List contents of a PHAR archive.";
	}

	static function phar_arg_list()
	{
		return array(
			'f' => array('type'=>'pharurl', 'val'=>NULL, 'required'=>1),
			);
	}

	static function phar_cmd_list()
	{
		global $args;
		
		foreach(new DirectoryTreeIterator($args['f']['val']) as $f)
		{
			echo "$f\n";
		}
	}
		
	static function phar_inf_trre()
	{
		return "Get a directory tree for a PHAR archive.";
	}

	static function phar_arg_tree()
	{
		return array(
			'f' => array('type'=>'pharurl', 'val'=>NULL, 'required'=>1),
			);
	}

	static function phar_cmd_tree()
	{
		global $args;
		
		foreach(new DirectoryGraphIterator($args['f']['val']) as $f)
		{
			echo "$f\n";
		}
	}
}

$r = new ReflectionClass('PharCommand');

foreach($r->getMethods() as $m)
{
	if (substr($m->name, 0, 9) == 'phar_cmd_')
	{
		$cmd = substr($m->name, 9);
		foreach(array('arg','cmd','inf') as $sub)
		{
			$func = 'phar_' . $sub . '_' . $cmd;
			if ($r->hasMethod($func))
			{
				$cmds[$cmd][$sub] = $m->class . '::' . $func;
			}
		}
	}
}

if ($argc < 2)
{
	echo "No command given, check ${argv[0]} help\n";
	exit(1);
}
elseif (!isset($cmds[$argv[1]]['cmd']))
{
	echo "Unknown command '${argv[1]}', check ${argv[0]} help\n";
	exit(1);
}
else
{
	$command = $argv[1];
}

if (isset($cmds[$command]['arg']))
{
	$args = call_user_func($cmds[$command]['arg']);
	$i = 1;
	$missing = false;
	while (++$i < $argc)
	{
		if ($argv[$i][0] == '-')
		{
			if (strlen($argv[$i]) == 2 && isset($args[$argv[$i][1]]))
			{
				$arg = $argv[$i][1];
				if (++$i >= $argc)
				{
					echo "Missing argument to parameter '$arg' of command '$command', check ${argv[0]} help\n";
					exit(1);
				}
				else
				{
					switch($type = $args[$arg]['type'])
					{
					case 'pharnew':
						
						break;
					case 'phar':
					case 'pharurl':
					case 'pharfile':
						try
						{
							$phar = $argv[$i];
							if (!file_exists($phar) && file_exists($ps = dirname(__FILE__).'/'.$phar))
							{
								$phar = $ps;
							}
							if (!Phar::loadPhar($phar))
							{
								"Unable to open phar '$phar'\n";
								exit(1);
							}
							switch($type)
							{
							case 'phar':
								$phar = new Phar($phar);
								break;
							case 'pharurl':
								$phar = 'phar://'.$phar;
								break;
							case 'pharfile':
							default:
								// just keep it
								break;
							}
						}
						catch(Exception $e)
						{
							echo "Exception while opening phar '${argv[$i]}':\n";
							echo $e->getMessage() . "\n";
							exit(1);
						}
						$args[$arg]['val'] = $phar;
						break;
					default:
						$args[$arg]['val'] = $argv[$i];
						break;
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
	if (isset($args['']))
	{
		if ($i >= $argc)
		{
			if (isset($args['']['require']) && $args['']['require'])
			{
				echo "Missing default trailing arguments to command $command, check ${argv[0]} help\n";
				$missing = true;
			}
		}
		else
		{
			$args['']['val'] = array();
			while($i < $argc)
			{
				$args['']['val'][] = $argv[$i++];
			}
		}
	}
	else if ($i < $argc)
	{
		echo "Unexpected default arguments to command $command, check ${argv[0]} help\n";
		exit(1);
	}
	foreach($args as $arg => $inf)
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
else
{
	$args = array();
}

call_user_func($cmds[$command]['cmd']);

?>