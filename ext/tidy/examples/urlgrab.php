<?php

    /*
     * urlgrab.php
     *
     * A simple command-line utility to extract all of the URLS contained
     * within <A HREF> tags from a document.
     *
     * NOTE: Only works with tidy for PHP 4.3.x, please see urlgrab5.php for tidy for PHP 5
     *
     * By: John Coggeshall <john@php.net>
     *
     * Usage: php urlgrab.php <file>
     *
     */
         
    /* Parse the document */
    tidy_parse_file($_SERVER['argv'][1]);
    
    /* Fix up the document */
    tidy_clean_repair();
    
    /* Get an object representing everything from the <HTML> tag in */
    $html = tidy_get_html();
    
    /* Traverse the document tree */
    print_r(get_links($html));
    
    function get_links($node) {
        $urls = array();
        
        /* Check to see if we are on an <A> tag or not */
        if($node->id == TIDY_TAG_A) {
            /* If we are, find the HREF attribute */
            $attrib = $node->get_attr(TIDY_ATTR_HREF);
            if($attrib) {
                /* Add the value of the HREF attrib to $urls */
                $urls[] = $attrib->value;
            }
            
        }
        
        /* Are there any children? */
        if($node->has_children()) {
            
            /* Traverse down each child recursively */
            foreach($node->children() as $child) {
                   
                /* Append the results from recursion to $urls */
                foreach(get_links($child) as $url) {
                    
                    $urls[] = $url;
                    
                }
                
            }
        }
        
        return $urls;
    }
    
?>  