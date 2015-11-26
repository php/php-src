--TEST--
Bug #49910: no support for ././@LongLink for long filenames in phar tar support
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("zlib")) die("skip"); ?>
--FILE--
<?php
$fname = str_replace('\\', '/', dirname(__FILE__) . '/files/Structures_Graph-1.0.3.tgz');
$tar = new PharData($fname);
$files = array();
foreach (new RecursiveIteratorIterator($tar) as $file) {
	$files[] = str_replace($fname, '*', $file->getPathName());
}
print_r($files);
?>
===DONE===
--EXPECT--
Array
(
    [0] => phar://*/Structures_Graph-1.0.3/LICENSE
    [1] => phar://*/Structures_Graph-1.0.3/Structures/Graph/Manipulator/AcyclicTest.php
    [2] => phar://*/Structures_Graph-1.0.3/Structures/Graph/Manipulator/TopologicalSorter.php
    [3] => phar://*/Structures_Graph-1.0.3/Structures/Graph/Node.php
    [4] => phar://*/Structures_Graph-1.0.3/Structures/Graph.php
    [5] => phar://*/Structures_Graph-1.0.3/docs/generate.sh
    [6] => phar://*/Structures_Graph-1.0.3/docs/html/Structures_Graph/Structures_Graph.html
    [7] => phar://*/Structures_Graph-1.0.3/docs/html/Structures_Graph/Structures_Graph_Manipulator_AcyclicTest.html
    [8] => phar://*/Structures_Graph-1.0.3/docs/html/Structures_Graph/Structures_Graph_Manipulator_TopologicalSorter.html
    [9] => phar://*/Structures_Graph-1.0.3/docs/html/Structures_Graph/Structures_Graph_Node.html
    [10] => phar://*/Structures_Graph-1.0.3/docs/html/Structures_Graph/_Structures_Graph_Manipulator_AcyclicTest_php.html
    [11] => phar://*/Structures_Graph-1.0.3/docs/html/Structures_Graph/_Structures_Graph_Manipulator_TopologicalSorter_php.html
    [12] => phar://*/Structures_Graph-1.0.3/docs/html/Structures_Graph/_Structures_Graph_Node_php.html
    [13] => phar://*/Structures_Graph-1.0.3/docs/html/Structures_Graph/_Structures_Graph_php.html
    [14] => phar://*/Structures_Graph-1.0.3/docs/html/Structures_Graph/tutorial_Structures_Graph.pkg.html
    [15] => phar://*/Structures_Graph-1.0.3/docs/html/classtrees_Structures_Graph.html
    [16] => phar://*/Structures_Graph-1.0.3/docs/html/elementindex.html
    [17] => phar://*/Structures_Graph-1.0.3/docs/html/elementindex_Structures_Graph.html
    [18] => phar://*/Structures_Graph-1.0.3/docs/html/errors.html
    [19] => phar://*/Structures_Graph-1.0.3/docs/html/index.html
    [20] => phar://*/Structures_Graph-1.0.3/docs/html/li_Structures_Graph.html
    [21] => phar://*/Structures_Graph-1.0.3/docs/html/media/banner.css
    [22] => phar://*/Structures_Graph-1.0.3/docs/html/media/stylesheet.css
    [23] => phar://*/Structures_Graph-1.0.3/docs/html/packages.html
    [24] => phar://*/Structures_Graph-1.0.3/docs/html/todolist.html
    [25] => phar://*/Structures_Graph-1.0.3/docs/tutorials/Structures_Graph/Structures_Graph.pkg
    [26] => phar://*/Structures_Graph-1.0.3/tests/AllTests.php
    [27] => phar://*/Structures_Graph-1.0.3/tests/testCase/BasicGraph.php
    [28] => phar://*/package.xml
)
===DONE===
