<?php

/** @file clicommand.inc
 * @ingroup Phar
 * @brief class CLICommand
 * @author  Marcus Boerger
 * @date    2007 - 2008
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
	protected $typs = array();

	function __construct($argc, array $argv)
	{
		$this->argc = $argc;
		$this->argv = $argv;
		$this->cmds = self::getCommands($this);
		$this->typs = self::getArgTyps($this);

		if ($argc < 2) {
			self::error("No command given, check ${argv[0]} help\n");
		} elseif (!isset($this->cmds[$argv[1]]['run'])) {
			self::error("Unknown command '${argv[1]}', check ${argv[0]} help\n");
		} else {
			$command = $argv[1];
		}

		if (isset($this->cmds[$command]['arg'])) {
			$this->args = call_user_func(array($this, $this->cmds[$command]['arg']));
			$i = 1;
			$missing = false;
			while (++$i < $argc) {
				if ($argv[$i][0] == '-') {
					if (strlen($argv[$i]) == 2 && isset($this->args[$argv[$i][1]])) {
						$arg = $argv[$i][1];
						if (++$i >= $argc) {
							self::error("Missing argument to parameter '$arg' of command '$command', check ${argv[0]} help\n");
						} else {
							$this->args[$arg]['val'] = $this->checkArgTyp($arg, $i, $argc, $argv);
						}
					}  else {
						self::error("Unknown parameter '${argv[$i]}' to command $command, check ${argv[0]} help\n");
					}
				} else {
					break;
				}
			}

			if (isset($this->args[''])) {
				if ($i >= $argc) {
					if (isset($this->args['']['require']) && $this->args['']['require']) {
						self::error("Missing default trailing arguments to command $command, check ${argv[0]} help\n");
					}
				} else {
					$this->args['']['val'] = array();
					while($i < $argc) {
						$this->args['']['val'][] = $argv[$i++];
					}
				}
			} else if ($i < $argc) {
				self::error("Unexpected default arguments to command $command, check ${argv[0]} help\n");
			}

			foreach($this->args as $arg => $inf) {
				if (strlen($arg) && !isset($inf['val']) && isset($inf['required']) && $inf['required']) {
					$missing .=  "Missing parameter '-$arg' to command $command, check ${argv[0]} help\n";
				}
			}

			if (strlen($missing)) {
				self::error($missing);
			}
		}

		call_user_func(array($this, $this->cmds[$command]['run']), $this->args);
	}

	static function notice ($msg)
	{
		fprintf(STDERR, $msg);
	}

	static function error ($msg, $exit_code = 1)
	{
		self::notice($msg);
		exit($exit_code);
	}

	function checkArgTyp($arg, $i, $argc, $argv)
	{
		$typ = $this->args[$arg]['typ'];

		if (isset($this->typs[$typ]['typ'])) {
			return call_user_func(array($this, $this->typs[$typ]['typ']), $argv[$i], $this->args[$arg], $arg);
		} else {
			return $argv[$i];
		}
	}

	static function getSubFuncs(CLICommand $cmdclass, $prefix, array $subs)
	{
		$a = array();
		$r = new ReflectionClass($cmdclass);
		$l = strlen($prefix);

		foreach($r->getMethods() as $m) {
			if (substr($m->name, 0, $l) == $prefix) {
				foreach($subs as $sub) {
					$what = substr($m->name, $l+strlen($sub)+1);
					$func = $prefix . $sub . '_' . $what;
					$what = str_replace('_', '-', $what);
					if ($r->hasMethod($func)) {
						if (!isset($a[$what])) {
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

	static function cli_arg_typ_bool($arg, $cfg, $key)
	{
		return (bool)$arg;
	}

	static function cli_arg_typ_int($arg, $cfg, $key)
	{
		if ((int)$arg != $arg) {
			self::error("Argument to -$key must be an integer.\n");
		}

		return (int)$arg;
	}

	static function cli_arg_typ_regex($arg, $cfg, $key)
	{
		if (strlen($arg)) {
			if (strlen($arg) > 1 && $arg[0] == $arg[strlen($arg)-1] && strpos('/,', $arg) !== false) {
				return $arg;
			} else {
				return '/' . $arg . '/';
			}
		} else {
			return NULL;
		}
	}

	static function cli_arg_typ_select($arg, $cfg, $key)
	{
		if (!in_array($arg, array_keys($cfg['select']))) {
			self::error("Parameter value '$arg' not one of '" . join("', '", array_keys($cfg['select'])) . "'.\n");
		}
		return $arg;
	}

	static function cli_arg_typ_dir($arg, $cfg, $key)
	{
		$f = realpath($arg);

		if ($f===false || !file_exists($f) || !is_dir($f)) {
			self::error("Requested path '$arg' does not exist.\n");
		}
		return $f;
	}

	static function cli_arg_typ_file($arg)
	{
		$f = new SplFileInfo($arg);
		$f = $f->getRealPath();
		if ($f===false || !file_exists($f)) {
			echo "Requested file '$arg' does not exist.\n";
			exit(1);
		}
		return $f;
	}

	static function cli_arg_typ_filenew($arg, $cfg, $key)
	{
		$d = dirname($arg);
		$f = realpath($d);

		if ($f === false) {
			self::error("Path for file '$arg' does not exist.\n");
		}
		return $f . '/' . basename($arg);
	}

	static function cli_arg_typ_filecont($arg, $cfg, $key)
	{
		return file_get_contents(self::cli_arg_typ_file($arg, $cfg, $key));
	}

	function cli_get_SP2($l1, $arg_inf)
	{
		return str_repeat(' ', $l1 + 2 + 4 + 8);
	}

	function cli_get_SP3($l1, $l2, $arg_inf)
	{
		return str_repeat(' ', $l1 + 2 + 4 + 8 + 2 + $l2 + 2);
	}

	static function cli_cmd_inf_help()
	{
		return "This help or help for a selected command.";
	}

	private function cli_wordwrap($what, $l, $sp)
	{
		$p = max(79 - $l, 40);     // minimum length for paragraph
		$b = substr($what, 0, $l); // strip out initial $l
		$r = substr($what, $l);    // remainder
		$r = str_replace("\n", "\n".$sp, $r); // in remainder replace \n's
		return $b . wordwrap($r, $p, "\n".$sp);
	}

	private function cli_help_get_args($func, $l, $sp, $required)
	{
		$inf = "";
		foreach(call_user_func($func, $l, $sp) as $arg => $conf) {
			if ((isset($conf['required']) && $conf['required']) != $required) {
				continue;
			}

			if (strlen($arg)) {
				$arg = "-$arg  ";
			} else {
				$arg = "... ";
			}

			$sp2 = $this->cli_get_SP2($l, $inf);
			$l2  = strlen($sp2);
			$inf .= $this->cli_wordwrap($sp . $arg . $conf['inf'], $l2, $sp2) . "\n";

			if (isset($conf['select']) && count($conf['select'])) {
				$ls = 0;
				foreach($conf['select'] as $opt => $what) {
					$ls = max($ls, strlen($opt));
				}
				$sp3 = $this->cli_get_SP3($l, $ls, $inf);
				$l3  = strlen($sp3);
				foreach($conf['select'] as $opt => $what) {
					$inf .= $this->cli_wordwrap($sp2 . "  " . sprintf("%-${ls}s  ", $opt) . $what, $l3, $sp3) . "\n";
				}
			}
		}
		if (strlen($inf)) {
			if ($required) {
				return $sp . "Required arguments:\n\n" . $inf;
			} else {
				return $sp . "Optional arguments:\n\n". $inf;
			}
		}
	}

	function cli_cmd_arg_help()
	{
		return array('' => array('typ'=>'any','val'=>NULL,'inf'=>'Optional command to retrieve help for.'));
	}

	function cli_cmd_run_help()
	{
		$argv  = $this->argv;
		$which = $this->args['']['val'];
		if (isset($which)) {
			if (count($which) != 1) {
				self::error("More than one command given.\n");
			}

			$which = $which[0];
			if (!array_key_exists($which, $this->cmds)) {
				if (strtolower($which) == 'commands') {
					self::cli_cmd_run_help_list();
					exit(0);
				}
				self::error("Unknown command, cannot retrieve help.\n");
			}

			$l = strlen($which);
			$cmds = array($which => $this->cmds[$which]);
		} else {
			echo "\n$argv[0] <command> [options]\n\n";
			$l = 0;
			ksort($this->cmds);
			foreach($this->cmds as $name => $funcs) {
				$l = max($l, strlen($name));
			}
			$inf = "Commands:";
			$lst = "";
			$ind = strlen($inf) + 1;
			foreach($this->cmds as $name => $funcs) {
				$lst .= ' ' . $name;
			}
			echo $this->cli_wordwrap($inf.$lst, $ind, str_repeat(' ', $ind)) . "\n\n";
			$cmds = $this->cmds;
		}
		$sp = str_repeat(' ', $l + 2);
		foreach($cmds as $name => $funcs) {
			$inf = $name . substr($sp, strlen($name));
			if (isset($funcs['inf'])) {
				$inf .= $this->cli_wordwrap(call_user_func(array($this, $funcs['inf'])), $l, $sp) . "\n";
				if (isset($funcs['arg'])) {
					$inf .= "\n";
					$inf .= $this->cli_help_get_args(array($this, $funcs['arg']), $l, $sp, true);
					$inf .= "\n";
					$inf .= $this->cli_help_get_args(array($this, $funcs['arg']), $l, $sp, false);
				}
			}
			echo "$inf\n\n";
		}
		exit(0);
	}

	static function cli_cmd_inf_help_list()
	{
		return "Lists available commands.";
	}

	function cli_cmd_run_help_list()
	{
		ksort($this->cmds);
		echo join(' ', array_keys($this->cmds)) . "\n";
	}
}

?>
