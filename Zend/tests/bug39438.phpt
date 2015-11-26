--TEST--
Bug #39438 (Fatal error: Out of memory)
--INI--
memory_limit=16M
--FILE--
<?php
$i=0;
$test2=array(
   'a1_teasermenu' => array(
   		'downloadcounter' => 2777,
        'versions' => array(
        	'0.1.0' => array (
        		'title' => 'A1 Teasermenu',
        	    'description' => 'Displays a teaser for advanced subpages or a selection of advanced pages',
        	    'state' => 'stable',
        	    'reviewstate' => 0,
        	    'category' => 'plugin',
        	    'downloadcounter' => 2787,
        	    'lastuploaddate' => 1088427240,
        	    'dependencies' => array (
        	          'depends' => array(
        	                  'typo3' =>'', 
        	                  'php' =>'', 
        	                  'cms' => ''
        	           ),        	
        	          'conflicts' => array('' =>'')        	
        	    ),        	
        	  	'authorname' => 'Mirko Balluff',
        	  	'authoremail' => 'balluff@amt1.de',
        	  	'ownerusername' => 'amt1',
        	  	't3xfilemd5' => '3a4ec198b6ea8d0bc2d69d9b7400398f',
      		)
  		)
  	)
);
$test=array();
while($i<1200) {	
	$test[]=$test2;
	$i++;
}
$out=serialize($test);
echo "ok\n";
?>
--EXPECT--
ok
