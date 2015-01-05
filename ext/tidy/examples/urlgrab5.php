<?php
    /*
     * urlgrab5.php
     *
     * A simple command-line utility to extract all of the URLS contained
     * within <A HREF> tags from a document.
     *
     * NOTE: Only works with tidy for PHP 5, please see urlgrab.php for tidy for PHP 4.3.x
     *
     * By: John Coggeshall <john@php.net>
     *
     * Usage: php urlgrab5.php <file>
     *
     */
    function dump_nodes(tidyNode $node, &$urls = NULL) {

	$urls = (is_array($urls)) ? $urls : array();
	
	if(isset($node->id)) {
	    if($node->id == TIDY_TAG_A) {
		$urls[] = $node->attribute['href'];
	    }
	}
		    
	if($node->hasChildren()) {

	    foreach($node->child as $c) {
		dump_nodes($c, $urls);
	    }

	}
	
	return $urls;
    }

    $a = tidy_parse_file($_SERVER['argv'][1]);
    $a->cleanRepair();
    print_r(dump_nodes($a->html()));
?>
