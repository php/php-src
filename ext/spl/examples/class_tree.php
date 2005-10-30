<?php

/** @file   class_tree.php
 * @brief   Class Tree example
 * @ingroup Examples
 * @author  Marcus Boerger
 * @date    2003 - 2005
 * @version 1.0
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
			if (strcasecmp(get_parent_class($cname), $base) == 0)
			{
				$this->offsetSet($cname, new SubClasses($cname));
			}
			if ($check_interfaces)
			{
				foreach(class_implements($cname) as $iname)
				{
					if (strcasecmp($iname, $base) == 0)
					{
						$this->offsetSet($cname, new SubClasses($cname));
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
	}

	/** @return key() since that is the name we need
	 */
	function current()
	{
		return parent::key();
	}
}

$it = new RecursiveTreeIterator(new SubClasses($argv[1], true));

echo $argv[1]."\n";
foreach($it as $c=>$v)
{
	echo "$v\n";
}

?>