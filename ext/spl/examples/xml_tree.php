<?php

/** @file   xml_tree.php
 * @brief   XML Tree example
 * @ingroup Examples
 * @author  Marcus Boerger
 * @date    2003 - 2005
 * @version 1.0
 *
 * Usage: php xml_tree.php \<file\> [\<len\>]
 *
 * Simply specify the xml file to tree with parameter \<file\>.
 * The second parameter allows to output a maximum of \<len\> characters of
 * the element or attribute value.
 */

if ($argc < 2) {
	echo <<<EOF
Usage: php ${_SERVER['PHP_SELF']} <file>

Displays a graphical tree for the elements in <file>.

<file> The file for which to generate the tree graph.
<len>  Max length to display for attribute and element content (default: 0).


EOF;
	exit(1);
}

if (!class_exists("RecursiveTreeIterator", false)) require_once("recursivetreeiterator.inc");

/** @ingroup Examples
 * @brief Generates a structure for a XML file
 * @version 1.0
 */
class SimpleXmlStructure extends AppendIterator implements RecursiveIterator
{
	/** @param xml object of class SimpleXmlStructure or XML file name to open.
	 */
	function __construct($xml)
	{
		parent::__construct();
		$xml = is_object($xml) ? $xml : $xml = simplexml_load_file($xml, 'SimpleXmlIterator');
		$this->append($xml);
		$this->append($xml->attributes());
	}

	function key()
	{
		return ($this->getIteratorIndex() ? '@' : '') . parent::key();
	}

	function hasChildren()
	{
		return $this->getInnerIterator()->hasChildren()
			|| !$this->getIteratorIndex();
	}

	function getChildren()
	{
		$inner = $this->getInnerIterator();
		return $this->getIteratorIndex()
		     ? $inner->getChildren()
		     : new SimpleXmlStructure($inner->getChildren());
	}
	
	function current()
	{
		global $len;
		
		if ($len)
		{
			$val = substr(parent::current(), 0, $len);
			$nws = preg_replace("/([\r\n]|\s\s+)/e", " ", $val);
			return ($this->getIteratorIndex() ? '= ' : ': ') . $nws;
		}
		else
		{
			return '';
		}
	}
}

$xml = new SimpleXmlStructure($argv[1]);
$it  = new RecursiveTreeIterator($xml, RecursiveTreeIterator::BYPASS_CURRENT);
$len = isset($argv[2]) ? $argv[2] : 0;

foreach($it as $c=>$v)
{
	echo "$c$v\n";
}

?>