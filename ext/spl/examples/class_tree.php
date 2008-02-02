<?php

/** @file   class_tree.php
 * @brief   Class Tree example
 * @ingroup Examples
 * @author  Marcus Boerger
 * @date    2003 - 2008
 * @version 1.1
 *
 * Usage: php class_tree.php \<class\>
 *
 * Simply specify the root class or interface to tree with parameter \<class\>.
 */

if ($argc < 2) {
	echo <<<EOF
Usage: php ${_SERVER['PHP_SELF']} <class>

Displays a graphical tree for the given <class>.

<class> The class or interface for which to generate the tree graph.


EOF;
	exit(1);
}

if (!class_exists("RecursiveTreeIterator", false)) require_once("recursivetreeiterator.inc");

/** \brief Collects sub classes for given class or interface
 */
class SubClasses extends RecursiveArrayIterator
{
	/** @param base  base class to collect sub classes for
	 * @param check_interfaces whether we deal with interfaces
	 */
	function __construct($base, $check_interfaces = false)
	{
		foreach(get_declared_classes() as $cname)
		{
			$parent = get_parent_class($cname);
			if (strcasecmp($parent, $base) == 0)
			{
				$this->offsetSet($cname, new SubClasses($cname));
			}
			if ($check_interfaces)
			{
				if ($parent)
				{
					$parent_imp = class_implements($parent);
				}
				foreach(class_implements($cname) as $iname)
				{
					if (strcasecmp($iname, $base) == 0)
					{
						if (!$parent || !in_array($iname, $parent_imp))
						{
							$this->offsetSet($cname, new SubClasses($cname));
						}
					}
				}
			}
		}
		if ($check_interfaces)
		{
			foreach(get_declared_interfaces() as $cname)
			{
				foreach(class_implements($cname) as $iname)
				{
					if (strcasecmp($iname, $base) == 0)
					{
						$this->offsetSet($cname, new SubClasses($cname, true));
					}
				}
			}
		}
		$this->uksort('strnatcasecmp');
	}

	/** @return key() since that is the name we need
	 */
	function current()
	{
		$result = parent::key();
		$parent = get_parent_class($result);
		if ($parent)
		{
			$interfaces = array_diff(class_implements($result), class_implements($parent));
			if ($interfaces)
			{
				$implements = array();
				foreach($interfaces as $interface)
				{
					$implements = array_merge($implements, class_implements($interface));
				}
				$interfaces = array_diff($interfaces, $implements);
				natcasesort($interfaces);
				$result .= ' (' . join(', ', $interfaces) . ')';
			}
		}
		return $result;
	}
}

$it = new RecursiveTreeIterator(new SubClasses($argv[1], true));

echo $argv[1]."\n";
foreach($it as $c=>$v)
{
	echo "$v\n";
}

?>