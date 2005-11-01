<?php

/** @file   xml_xpath_tree.php
 * @brief   XML XPath Tree example
 * @ingroup Examples
 * @author  Marcus Boerger
 * @date    2003 - 2005
 * @version 1.0
 *
 * Usage: php xml_tree.php \<file\> \<xpath\> [\<len\>]
 *
 * Simply specify the xml file to tree with parameter \<file\>.
 *
 * The second parameter is a xpath expression to be evaluated.
 *
 * The third parameter allows to output a maximum of \<len\> characters of
 * the element or attribute value.
 */

if ($argc < 3) {
	echo <<<EOF
Usage: php ${_SERVER['PHP_SELF']} <file>

Displays a graphical tree for the elements in <file> <xpath> [<len>].

<file>  The file for which to generate the tree graph.
<xpath> An XPath expression to evaluate.
<len>   Max length to display for attribute and element content (default: 0).


EOF;
	exit(1);
}

if (!class_exists("RecursiveTreeIterator", false)) require_once("recursivetreeiterator.inc");

/** @ingroup Examples
 * @brief Generates the XML structure for SimpleXMLXPathResult elements
 * @version 1.0
 */
class SimpleXpathStructure extends AppendIterator implements RecursiveIterator
{
	function __construct($xml)
	{
		parent::__construct();
		if (!is_null($xml))
		{
			$xml = is_object($xml) ? $xml : $xml = simplexml_load_file($xml, 'SimpleXmlIterator');
			$this->append($xml);
			$attr = $xml->attributes();
			if ($attr)
			{
				$this->append($attr);
			}
		}
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

/** @ingroup Examples
 * @brief Generates a structure from an xpath query on a XML file
 * @version 1.0
 */
class SimpleXMLXPathResult extends RecursiveArrayIterator
{
	/** 
	 * @param $file XML file to open
	 * @param $xpath query to execute
	 */
	function __construct($file, $xpath)
	{
		$xml = @simplexml_load_file($file, 'SimpleXmlIterator');
		parent::__construct($xml ? $xml->xpath($xpath) : NULL);
	}

	function current()
	{
		return '';
	}

	function hasChildren()
	{
		return true;
	}

	function getChildren()
	{
		return new SimpleXpathStructure(parent::current());
	}
}

$len = isset($argv[3]) ? $argv[3] : 0;
$xml = new SimpleXMLXPathResult($argv[1], $argv[2]);
$it  = new RecursiveTreeIterator($xml, RecursiveTreeIterator::BYPASS_CURRENT);
foreach($it as $c=>$v)
{
	echo "$c$v\n";
}

?>