<?php


$usage = <<<USAGE

Usage: php find_tested.php [path_to_test_files] ([extension])

Outputs test coverage information for functions and methods in csv format.
Supplying an optional extension name outputs only information for functions and methods from that extension.

Output format:
Extension, Class Name, Method/Function Name, Test Status, Test Files

A test status of "verify" for a method means that there is at least one other method of the same name, so test coverage must be verified manually.

USAGE;


/* method record fields */
define("CLASS_NAME", "CLASS_NAME");
define("METHOD_NAME", "METHOD_NAME");
define("EXTENSION_NAME", "EXTENSION_NAME");
define("IS_DUPLICATE", "IS_DUPLICATE");
define("IS_TESTED", "IS_TESTED");
define("TESTS", "TESTS");


// process command line args
$num_params = $argc;
if ($num_params < 2 || $num_params > 3) {
    die($usage);
}

$extension_test_path = $argv[1];

if ($num_params == 3) {
    $extension_name = $argv[2];

    // check extension exists
    $extensions = get_loaded_extensions();
    if (!in_array($extension_name, $extensions)) {
        echo "Error: extension $extension_name is not loaded. Loaded extensions:\n";
        foreach($extensions as $extension) {
            echo "$extension\n";
        }
        die();
    }
} else {
    $extension_name = false;
}


$method_info = populate_method_info();

if ($extension_name != false) {
    // get only the methods from the extension we are querying
    $extension_method_info = array();
    foreach($method_info as $method_record) {
        if (strcasecmp($extension_name, $method_record[EXTENSION_NAME]) == 0) {
            $extension_method_info[] = $method_record;
        }
    }
} else {
    $extension_method_info = $method_info;
}

get_phpt_files($extension_test_path, $count, $phpt_files);

$extension_method_info = mark_methods_as_tested($extension_method_info, $phpt_files);

/**
 * The loop to ouput the test coverage info
 * Should output: Extension, Class Name, Method/Function Name, Test Status, Test Files
 */ 
foreach($extension_method_info as $record) {
    echo $record[EXTENSION_NAME] . ",";
    echo $record[CLASS_NAME] . ",";
    echo $record[METHOD_NAME] . ",";
    echo $record[IS_TESTED] . ",";
    echo $record[TESTS] . "\n";
}

/**
 * Marks the "tested" status of methods in $method_info according
 * to whether they are tested in $phpt_files
 */
function mark_methods_as_tested($method_info, $phpt_files) { 

    foreach($phpt_files as $phpt_file) {
        $tested_functions = extract_tests($phpt_file);

        foreach($tested_functions as $tested_function) {

            // go through method info array marking this function as tested
            foreach($method_info as &$current_method_record) {
                if (strcasecmp($tested_function, $current_method_record[METHOD_NAME]) == 0) {
                    // matched the method name
                    if ($current_method_record[IS_DUPLICATE] == true) {
                        // we cannot be sure which class this method corresponds to,
                        // so mark method as needing to be verified 
                        $current_method_record[IS_TESTED] = "verify";
                    } else {
                        $current_method_record[IS_TESTED] = "yes";
                    }
                    $current_method_record[TESTS] .= $phpt_file . "; ";
                }
            }
        }
    }
    return $method_info;
}

/**
 * returns an array containing a record for each defined method.
 */
function populate_method_info() {
    
    $method_info = array();

    // get functions
    $all_functions = get_defined_functions();
    $internal_functions = $all_functions["internal"];

    foreach ($internal_functions as $function) {
        // populate new method record
        $function_record = array();
        $function_record[CLASS_NAME] = "Function";
        $function_record[METHOD_NAME] = $function;
        $function_record[IS_TESTED] = "no";
        $function_record[TESTS] = "";
        $function_record[IS_DUPLICATE] = false;

        // record the extension that the function belongs to
        $reflectionFunction = new ReflectionFunction($function);
        $extension = $reflectionFunction->getExtension();
        if ($extension != null) {
            $function_record[EXTENSION_NAME] = $extension->getName();
        } else {
            $function_record[EXTENSION_NAME] = "";
        }
        // insert new method record into info array
        $method_info[] = $function_record;
    }

    // get methods
    $all_classes = get_declared_classes();
    foreach ($all_classes as $class) {
        $reflectionClass = new ReflectionClass($class);
        $methods = $reflectionClass->getMethods();
        foreach ($methods as $method) {
            // populate new method record
            $new_method_record = array();
            $new_method_record[CLASS_NAME] = $reflectionClass->getName();
            $new_method_record[METHOD_NAME] = $method->getName();
            $new_method_record[IS_TESTED] = "no";
            $new_method_record[TESTS] = "";

            $extension = $reflectionClass->getExtension();
            if ($extension != null) {
                $new_method_record[EXTENSION_NAME] = $extension->getName();
            } else {
                $new_method_record[EXTENSION_NAME] = "";
            }

            // check for duplicate method names
            $new_method_record[IS_DUPLICATE] = false;
            foreach ($method_info as &$current_record) {
                if (strcmp($current_record[METHOD_NAME], $new_method_record[METHOD_NAME]) == 0) {
                    $new_method_record[IS_DUPLICATE] = true;
                    $current_record[IS_DUPLICATE] = true;
                }
            }
            // insert new method record into info array
            $method_info[] = $new_method_record;
        }
    }

    return $method_info;
}

function get_phpt_files($dir, &$phpt_file_count, &$all_phpt)
{
    $thisdir = dir($dir.'/'); //include the trailing slash
    while(($file = $thisdir->read()) !== false) {
        if ($file != '.' && $file != '..') {
            $path = $thisdir->path.$file;
            if(is_dir($path) == true) {
                get_phpt_files($path , $phpt_file_count , $all_phpt);
            } else {
                if (preg_match("/\w+\.phpt$/", $file)) {
                    $all_phpt[$phpt_file_count] = $path;
                    $phpt_file_count++;
                }
            }
        }
    }
}

/**
 * Extract tests from a specified file, returns an array of tested function tokens 
 */ 
function extract_tests($file) {
	$code = file_get_contents($file);
	
	if (!preg_match('/--FILE--\s*(.*)\s*--(EXPECTF|EXPECTREGEX|EXPECT)?--/is', $code, $r)) {
		//print "Unable to get code in ".$file."\n";
		return array();
	}
	
	$tokens = token_get_all($r[1]);
	$functions = array_filter($tokens, 'filter_functions');
	$functions = array_map( 'map_token_value',$functions);
	$functions = array_unique($functions);
	
	return $functions;
}

function filter_functions($x) {
	return $x[0] == 307;
}

function map_token_value($x) {
	return $x[1];
}


?>

