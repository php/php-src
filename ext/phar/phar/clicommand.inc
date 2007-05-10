<?php

/** @file clicommand.inc
 * @ingroup Phar
 * @brief class CLICommand
 * @author  Marcus Boerger
 * @date    2007 - 2007
 *
 * Phar Command
 */

/** @ingroup Phar
 * @brief   Abstract base console command implementation
 * @author  Marcus Boerger
 * @version 1.0
 */
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
								$this->args[$arg]['val'] = call_user_func(array($this, $this->types[$type]['typ']), $argv[$i], $this->args[$arg]);
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

	static function cli_arg_typ_file($arg)
	{
		if (!file_exists($arg))
		{
			echo "Requested file '$arg' does not exist.\n";
			exit(1);
		}
		return $arg;
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

		echo "
$argv[0] <command> [options]

Commands:


";
		$l = 0;
		foreach($this->cmds as $name => $funcs)
		{
			$l = max($l, strlen($name));
		}
		$sp = str_repeat(' ', $l+2);
		foreach($this->cmds as $name => $funcs)
		{
			$inf = sprintf("%${l}s  ", $name);
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
						if ($conf['type'] == 'select')
						{
							$l2 = 0;
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

?>