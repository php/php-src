<?php
/* This is a script which generates simple PHPT test cases from the name of the function.
 * It works using the {{{ proto for the function PHP source code. The test cases that it generates 
 * are simple, however you can also give it the name of a file with PHP code in and it will turn this
 * into the right format for a PHPT test. 
 * This script will not generate expected output. 
 * Further quidance on how to use it can be found on qa.php.net, or by using the -h command line option.
 */

//read the command line input and parse it, do some basic checks to make sure that it's correct

$opt = initialise_opt();
$opt = parse_args($argv, $opt);

check_source($opt['source_loc']);
check_fname($opt['name']);
check_testcase($opt['error_gen'], $opt['basic_gen'],$opt['variation_gen']);

if ($opt['include_block'] != NULL) {
        check_file($opt['include_block']);
}
 

//Get a list of all the c funtions in the source tree

$all_c = array();
$c_file_count = 0;
dirlist($opt['source_loc'], $c_file_count, $all_c);


//Search the list of c functions for the function prototype, quit if can't find it or if the function is an alias

$test_info = get_loc_proto($all_c, $opt['name'], $opt['source_loc']);

if (!$test_info['found']) {
	echo "Exiting: Unable to find implementation of {$opt['name']} in {$opt['source_loc']}\n";
	exit();
}
if ($test_info['falias']) {
	//If this function is  falias'd to another function direct the test case writer to the principle alias for tests
	echo "{$test_info['name']}() is an alias of {$test_info['alias']}() --- please see testcases for {$test_info['alias']}() \n";
	exit();
}
if ($test_info['error'] != NULL) {
	echo $test_info['error']."\n";
	exit();
}

$test_info['proto'] = "proto ".$test_info['return_type']." ". $test_info['name']."(".$test_info['params'].")";


//Set the test sections array - may want more in this later?

$sections = array('--TEST--', 
			'--FILE--', 
			'--EXPECTF--' 
			);

//Parse the parameter list to get the information we need to build code

$names = array();
$types = array();
$num_opt_args = 0;
$num_args = 0;

$parse_success = parse_params($test_info['params'], $names, $types, $num_args, $num_opt_args);

if(!$parse_success) { 
	echo "Unable to parse function parameter list: {$test_info['params']}. Will only generate template test\n";
}

if((!$parse_success) || ($opt['include_block'] != NULL)) {
	//EITHER 
	//     	parse_params returns false (failed) so just generate template code
	// OR
	//      If the user has given a file to import PHP code from - don't attempt to generate code
	//
	echo "\nGenerating test case for ".$test_info['name']."()\n\n";
	if($opt['basic_gen']) {
        	$test_case = gen_template($test_info, $sections, 'basic', $opt['include_block']);
	}
	elseif ($opt['error_gen']) {
        	$test_case = gen_template($test_info, $sections, 'error', $opt['include_block']);
	}
	elseif ($opt['variation_gen']) {
        	$test_case = gen_template($test_info, $sections, 'variation', $opt['include_block']);
	}
	exit();
}


// parse params succeded - so set up the function arguments to be used in test generation

$test_info['arg_c'] = $num_args;
$test_info['optional_args'] = $num_opt_args;
if ($num_args > 0) {
	$test_info['arg_det'] = array_combine($names, $types);
}

/* DEBUG HERE
	var_dump($test_info);
	exit();
*/


// Ready to generate test cases as required

echo "\nGenerating test case for ".$test_info['name']."()\n\n";
$test_case = array();
if($opt['basic_gen']) {
	$test_case = gen_basic_test($test_info, $sections, $test_case);
}
elseif ($opt['error_gen']) {
	$test_case = gen_error_test($test_info, $sections, $test_case);
}
elseif ($opt['variation_gen']) {
	//generates a number of tests 
	gen_variation_test($test_info, $sections);
}
/*
 * END OF MAIN CODE SECTION
 */
/*
 * Function to read the contents of a PHP into an array
 * Arguments:
 *	File name => file name with PHP code in it
 * 	$test_case => test case array to be appended to
 * Returns:
 *	$test_case
 */
function read_include_file($file_name, $test_case) {
	$fp = fopen($file_name, "r");
	
	$code_block = file($file_name);
	
	fclose($fp);
	
	//strip PHP tags and blank lines from start of file
	foreach ($code_block as $line) {
		if (preg_match("/<\?php/", $line)){
			array_shift($code_block);
			break;
		}else if(preg_match("/^\s*$/",$line)) {
			array_shift($code_block);
		}else {
			break;
		}
	}
	
	//Strip PHP tags and blank lines from the end of the code
	$last = count($code_block) -1;	
	for($j = 0; $j <= $last; $j++) {
		$i = $last - $j;
		$line = $code_block[$i];
		if(preg_match("/\?>/", $line)) {
			array_pop($code_block);
			break;
		}else if(preg_match("/^\s*$/",$line)) {
			array_pop($code_block);
		}else {
			break;
		}

	}

	// Append the lines of code to the test case array and return
	foreach($code_block as $line) {
		array_push($test_case, $line);
	}
	return $test_case;
}
		
/* 
 * Generates basic functionality testcase and writes it out to a file.
 * Arguments:
 *	$fn_det =>  array containing details of the function,
 *	$sections => The test case sections (eg --TEST--) as an array
 * 		The test case code as arrays keyed by section.
 * Returns:
 * 	The test case code as an array of arrays, indexed by section                
 */
function gen_basic_test($fn_det, $sections, $test_case) {
	$name = $fn_det['name'];
	$proto = $fn_det['proto'];
	$desc = $fn_det['desc'];
	$source_file = $fn_det['source_file'];
	$arg_det = $fn_det['arg_det'];
	$arg_c = $fn_det['arg_c'];
	$optional_args = $fn_det['optional_args'];
	$alias = $fn_det['alias'];

	// get the test header
	$test_case = gen_test_header($name, $proto, $desc, $source_file, "basic functionality", NULL, $alias, $test_case);

	$test_case['--FILE--'] = gen_basic_test_code($name, $arg_det, $arg_c, $optional_args, $test_case['--FILE--']);

	//End the script
	$test_case = gen_test_trailer($test_case,'--EXPECTF--');
	write_file($test_case, $name, 'basic', $sections);
	return($test_case);
}
/* 
 * Function to scan recursively down a directory structure looking for all c files.
 * Input:
 * 	$dir - string path of top level directory
 * 	&$c_file_count - reference to a count of the number of files - init to 0, is updated with count after code is run
 * 	&$all_c - reference to list of all c files. Initialise to array, will contain file list after code is run
 * Output:
 * 	$all_c (see above)
 * 	$c_file_count (see above)
 */
function dirlist($dir, &$c_file_count, &$all_c)
{
	$thisdir = dir($dir.'/'); //include the trailing slash
	while(($file = $thisdir->read()) !== false) {
		if ($file != '.' && $file != '..') {
          		$path = $thisdir->path.$file;
          			if(is_dir($path) == true) {
                     			dirlist($path , $c_file_count , $all_c);
            			} else {
                 			if (preg_match("/\w+\.c$/", $file)) {
               	      				$all_c[$c_file_count] = $path;
               	      				$c_file_count++;
                 			}
            			}
        	}
    	}
return;
}
/*
 * Function takes parameter list extracted from the proto comment and retuns a list 
 * of names and types
 * Arguments:
 * 	$param_list (string) = > list of arguments and types
 * 	$names => reference to an array of variable names
 * 	$types => reference to an array of variable
 * 	$nm_args => reference to number of function argumants
 * 	$num_opt_args => reference to number of optional arguments
 * Returns:
 * 	True if have been able to parse string $param_list, false if not.
 */ 
function parse_params($param_list, &$names, &$types, &$num_args, &$num_opt_args) {
	$opt_args = false;
	$num_mand_args =0;
	$num_opt_args = 0;
	$num_args = 0;
	$opt_params = NULL;

        //remove all commas
        $param_list = preg_replace("/,/", "", $param_list);

        //deal with void
        if(preg_match("/\s*void\s*/", $param_list)) {
                return true; 
        }

        // extract mandatory parameters and  optional parameters
        if (preg_match("/^(.*?)\[\s*(.*)\]\s*(.*?)$/", $param_list, $matches)) {
                $param_list = $matches[1].$matches[3];
                $opt_params = $matches[2];

                // Extract nested optional parameters
		$temp_opt_params = $opt_params;
                while (preg_match("/(.+?)\s*\[\s*(.*)\]/",$temp_opt_params, $matches)) {
                	$opt_params = $matches[1]." ".$matches[2];
                	$temp_opt_params = $opt_params;
                }
        }

        // seperate parameter list into array of types and names 
	if ($param_list != "") {
       		$param_list = chop($param_list);
       		$param_array = explode(" ", $param_list);

		$num_mand_args = count($param_array)/2;
		//check that this is an even number and quit if not, means we have failed to parse correctly
		if((round($num_mand_args) * 2) != count($param_array)) {return false;}

       		$j = 0;
       		for($i=0; $i<count($param_array); $i=$i+2) {
			$j = $i + 1;
			$types[$i] = $param_array[$i];
			// If the variable is a reference remove the & from the variable name
			$names[$i] = preg_replace("/&/", "", $param_array[$j]);
       		}
	}
        //initialise optional arguments
        if ($opt_params != NULL) {
	       	$opt_array = explode(" ", $opt_params);

		$num_opt_args = count($opt_array)/2;
		//check that this is an even number and quit if not, means we have failed to parse correctly
		if((round($num_opt_args) * 2) != count($opt_array)) {return false;}

        	$j = 0;
       		for($i=0; $i<count($opt_array); $i=$i+2) {
				$j = $i + 1;
				array_push($types, $opt_array[$i]);
				// If the variable is a reference remove the & from the variable name
				array_push($names, preg_replace("/&/", "", $opt_array[$j]));
            	}
        }
	$num_args = $num_mand_args + $num_opt_args;
	return true;
}
/*
 * Generates code for an array which contains invalid data types.
 * For example, if the variable being tested is of type "float" this code will 
 * generate an array of data whose type does not include float
 *  
 * Arguments:
 * 	$array_name => name of the array that should be returned
 * 	$var_type =>  data type of the argument
 * 	Array of code - will be appended to
 * Returns:
 * 	$code_block with appended lines of code to initialse the array of data
 */
function gen_array_with_diff_values($var_type,  $array_name, $code_block) {
	//generate the array with all different values, skip those of the same type as $var_type 

	// integer values
	$variation_array['int'] = array(
		"0",
  		"1",
  		"12345",
  		"-2345"
		);

	// float values
	$variation_array['float'] = array(
  		"10.5",
  		"-10.5",
  		"10.5e10",
  		"10.6E-10",
  		".5"
		);

	// array values
	$variation_array['array'] = array(
		"array()",
		"array(0)",
		"array(1)",
		"array(1, 2)",
		"array('color' => 'red', 'item' => 'pen')"
		);

	// null vlaues
	$variation_array['null'] = array(
		"NULL",
		"null"
		);

	// boolean values
	$variation_array['boolean'] = array(
		"true",
		"false",
		"TRUE",
		"FALSE"
		);

	// empty string
	$variation_array['empty'] = array(
		"\"\"",
		"''",
		);

	// string values
	$variation_array['string'] = array(
		"\"string\"",
		"'string'",
		);

	// objects
	$variation_array['object'] = array(
  		"new stdclass()"
		);


	// undefined variable
	$variation_array['undefined'] = array(
  		'$undefined_var'
		);

	// unset variable
	$variation_array['unset'] = array(
		'$unset_var'
		);


 	//Write out the code block for the variation array
        $blank_line = "";
        array_push($code_block, "\$$array_name = array(");
        foreach ($variation_array as $type => $data) {
                if($type != $var_type) {
                        array_push($code_block, $blank_line);
                        $comment = "      // $type data";
                        array_push($code_block,$comment);
                        foreach ($variation_array[$type] as $entry) {
                                $line = "      ".$entry.",";
                                array_push($code_block, $line);
                        }
                }
        }
	array_push($code_block, ");");
	return $code_block;
}

/*
 * Generate variation testcases and writes them to file(s)
 * 	1) generate variation for each argument where different invalid argument values are passed
 * 	2) generate a vartiation template
 * Arguments:
 * 	$fn_det => array containing details of the function,
 * 	$sections => list of test sections, eg '--TEST--', etc
 * Returns:
 * 	Nothing at the moment - should be tru for success/false for fail?
 * 
*/
function gen_variation_test($fn_det, $sections) {

        $name = $fn_det['name'];
        $proto = $fn_det['proto'];
        $desc = $fn_det['desc'];
        $source_file = $fn_det['source_file'];
        $arg_det = $fn_det['arg_det'];
        $arg_c = $fn_det['arg_c'];
        $optional_args = $fn_det['optional_args'];
        $alias = $fn_det['alias'];

	$test_case = array();

	$test_case = gen_template($fn_det, $sections, 'variation');

	// if the function has zero argument then quit here because we only need the template
	if($arg_c == 0) {
		return;
	}

	// generate a sequence of other tests which loop over each function arg trying different values
	$name_seq = 1;
	$arg_count = 1;
	if($arg_c > 0) {
		for($i = 0; $i < $arg_c; $i++) {
			//generate a different variation test case for each argument
			$test_case = array();

			$test_case = gen_test_header($name, $proto, $desc, 
                                     $source_file, "usage variations", " - <type here specifics of this variation>", $alias, $test_case);

			// add variation code
			$test_case['--FILE--'] = gen_variation_diff_arg_values_test($name, $arg_det, $arg_count, $test_case['--FILE--']);

			// end the script
			$test_case = gen_test_trailer($test_case, '--EXPECTF--');
			$tc_name = 'variation'.$name_seq;
			write_file($test_case, $name, $tc_name, $sections);

			$arg_count ++; // next time generate the code for next argument of the function;
			$name_seq ++; // next seqence number for variation test name
		}
	}
}
/* 
 * Generate code for testcase header. The following test sections are added: 
 * --TEST--  & --FILE--
 * Arguments:
 * 	$fn_name => name of the function
 * 	$proto => $fn_name function prototype
 * 	$desc => short description of $fn_name function
 * 	$source_file => location of the file that implements $fn_name function
 * 	$type_msg => Message to indicate what type of testing is being done : "error_conditions", "basic functionality", etc
 * 	$extra_msg => Additional message that will be printed to indicate what specifics are being tested in this file. 
 * 	$alias => list any functions that are aliased to this
 * 	$test_sections => an array of arays of testcase code, keyed by section
 * Returns:
 * 	$test_sections
 */ 
function gen_test_header($fn_name, $proto, $desc, $source_file, $type_msg, $extra_msg, $alias, $test_sections) {
	$msg = "$type_msg";
	if($extra_msg != NULL)
  		$msg = "$msg - $extra_msg";



	$test_sections['--TEST--'] = array("Test $fn_name() function : $type_msg $extra_msg"
				);

	$test_sections['--FILE--'] = array ("<?php",
					"/* Prototype  : $proto",
					" * Description: $desc",
					" * Source code: $source_file",
					" * Alias to functions: $alias",
 					" */",
 					"",
 					"/*",
					" * add a comment here to say what the test is supposed to do",
 					" */",
 					"",
					"echo \"*** Testing $fn_name() : $type_msg ***\\n\";",
					""
					);

	return $test_sections;
}
/*
 * Generate error testcase and writes it to a file
 * 	1. Generates more than expected no. of argument case
 * 	2. Generates less than expected no. of argument case
 * Arguments:
 * 	$fn_det =>  array containing details of the function
 * 	$sections => The test case sections (eg --TEST--) as amn array
 * 	$test_case => The test case code as arrays keyed by section.
 * Returns:
 * 	The test case code as an array of arrays, indexed by section
 */
function gen_error_test($fn_det, $sections, $test_case) {
        $name = $fn_det['name'];
        $proto = $fn_det['proto'];
        $desc = $fn_det['desc'];
        $source_file = $fn_det['source_file'];
        $arg_det = $fn_det['arg_det'];
        $arg_c = $fn_det['arg_c'];
        $optional_args = $fn_det['optional_args'];
        $alias = $fn_det['alias'];


	// get the test header 
	$test_case = gen_test_header($name, $proto, $desc, $source_file, "error conditions", NULL, $alias, $test_case);

	if($fn_det['arg_c'] == 0 ) {
		//If the function expects zero arguments generate a one arg test case and quit
		$test_case['--FILE--'] = gen_one_arg_code($name, "extra_arg", "int" , $test_case['--FILE--']);

	} else if (($fn_det['arg_c'] - $fn_det['optional_args']) == 1) { 
		//If the function expects one argument generate a zero arg test case and two arg test case 
		$test_case['--FILE--'] = gen_zero_arg_error_case($name, $test_case['--FILE--']);
		$test_case['--FILE--'] = gen_morethanexpected_arg_error_case($name, $arg_det, $test_case['--FILE--']);
	} else {
		$test_case['--FILE--'] = gen_morethanexpected_arg_error_case($name, $arg_det, $test_case['--FILE--']);
		$test_case['--FILE--'] = gen_lessthanexpected_arg_error_case($name, $arg_det, $arg_c, $optional_args, $test_case['--FILE--']);
	}
	// End the script
	$test_case = gen_test_trailer($test_case, '--EXPECTF--');
	write_file($test_case, $name, 'error', $sections);
	return($test_case);
}
/*
 * Add the final lines of the testcase, the default is set to be EXPECTF.
 * Arguments: 
 * 	$test_case - An aray of arrays keyed by test section
 * 	$section_key - Type of EXPECT section, defaults to EXPECTF
 * Returns:
 * 	$test_case - completed test cases code as an array of arrays keyed by section
 */
function gen_test_trailer($test_case, $section_key = "--EXPECTF--") {
	//Complete the --FILE-- section
	array_push($test_case['--FILE--'], "");
	array_push($test_case['--FILE--'], "echo \"Done\";", "?>");

	//add a new key for the expect section
	$test_case[$section_key]=array();
	array_push($test_case[$section_key], "Expected output goes here"); 
	array_push($test_case[$section_key], "Done"); 

	return $test_case;
}
/*
 * Writes test case code to a file
 * Arguments:
 * 	$test_case => Array of arrays of test sections, keyed by section
 * 	$function_name => Name of functio that tests are being generated for
 * 	$type => basic/error/variation
 * 	$test_sections => keys to $test_case
 * 	$seq = > sequence number, may be appended to file name
 * Returns:
 * 	Nothing at the moment - should be true/false depending on success
 */
function write_file($test_case, $function_name, $type, $test_sections, $seq="") {
	$file_name = $function_name."_".$type.$seq.".phpt";

	$fp = fopen($file_name, 'w');
	
	foreach($test_sections as $section) {
		if(array_key_exists($section, $test_case)) {
		fwrite($fp, $section."\n");
       			if(count($test_case[$section]) >0 ){
                		foreach($test_case[$section] as $line_of_code) {
					fwrite($fp, $line_of_code."\n");
				}
                	}
		}
        }
	fclose($fp);
}
/* 
 * Generate code for testing different invalid values against an argument of the function 
 * Arguments:
 * 	$fn_name => name of the function
 * 	$arg_det => details of the each argument, stored in an array in the form of 'nameofargument' => 'typeofargument'
 * 	$which_arg => a numeric value starting from 1 indicating the argument of the 
 * 		function ( in $arg_det ) for which the case needs to be generated
 * 	$code_block => array of code which will be appended to
 * Returns:
 * 	$code_block
 */
function gen_variation_diff_arg_values_test($fn_name, $arg_det, $which_arg, $code_block) {

	$names = array_keys($arg_det);
	$types = array_values($arg_det);

	$blank_line = "";

	// decrement the $which_arg so that its matches with  the index of $types
	$which_arg--;

	// generate code to initialise arguments that won't be substituted
	array_push($code_block, "// Initialise function arguments not being substituted (if any)");
	for($i = 0; $i < count($types); $i ++) {
		if ($i != $which_arg) { // do not generate initialization code for the argument which is being tested
			$i_stmt = get_variable_init_statement($types[$i], $names[$i]);
                	array_push($code_block, $i_stmt);
		}
	}
	array_push($code_block, $blank_line);

	// generate code to unset a variable
	array_push($code_block, "//get an unset variable");
	array_push($code_block, "\$unset_var = 10;");
	array_push($code_block, "unset (\$unset_var);");
	array_push($code_block, $blank_line);

	//generate code for an array of values to iterate over
	array_push($code_block, "//array of values to iterate over");
	$code_block = gen_array_with_diff_values($types[$which_arg], 'values', $code_block);

	//generate code for loop to iterate over array values
	array_push($code_block, $blank_line);

	array_push($code_block, "// loop through each element of the array for $names[$which_arg]");
	array_push($code_block, $blank_line);
	
	array_push($code_block, "foreach(\$values as \$value) {");
	array_push($code_block, "      echo \"\\nArg value \$value \\n\";");

        // prepare the function call

        // use all arguments including the optional ones to construct a single function call
        $var_name = array();
        foreach ($names as $nm) {
                array_push($var_name, "$".$nm);
        }
	$var_name[$which_arg] = "\$value";
        $all_args = implode(", ", $var_name);

        array_push ($code_block, "      var_dump( $fn_name($all_args) );");
	array_push ($code_block, "};");

  
	return $code_block;
}
/*
 * Generate code for testing more than expected no. of argument for error testcase
 * Arguments:
 *  	$fn_name => name of the function
 *  	$arg_det => details of the each argument, stored in an array in the form of 'nameofargument' => 'typeofargument'
 *  	$code_block => an array of code that will be appended to
 * Returns:
 *  	$code_block
 */
function gen_morethanexpected_arg_error_case($fn_name, $arg_det ,$code_block) {
	array_push($code_block, "");
	array_push($code_block, "//Test $fn_name with one more than the expected number of arguments");
	array_push($code_block, "echo \"\\n-- Testing $fn_name() function with more than expected no. of arguments --\\n\";");

	$names = array_keys($arg_det);
	$types = array_values($arg_det);
  
	//Initialise expected arguments
	for($i = 0; $i < count($types); $i ++) {
		$i_stmt = get_variable_init_statement($types[$i], $names[$i]); 
		array_push($code_block, $i_stmt); 
	}

	// get the extra argument init statement
	$i_stmt = get_variable_init_statement("int", "extra_arg"); 
	array_push($code_block, $i_stmt); 

	$var_name = array();
	foreach ($names as $nm) {
		array_push($var_name, "$".$nm);
	}
	$all_args = implode(", ", $var_name);

	array_push($code_block, "var_dump( $fn_name($all_args, \$extra_arg) );");

	return $code_block;
}

/* 
 * Generate code for testing less than expected no. of mandatory arguments for error testcase
 * Arguments:
 *  	$fn_name => name of the function
 *  	$arg_det => details of the each argument, stored in an array in the form of 'nameofargument' => 'typeofargument'
 *  	$arg_c =>  total count of arguments that $fn_name takes
 *  	$optional_args => total count of optional arguments that $fn_name takes
 *  	$code_block => an array of code that will be appended to
 * Returns:
 *  	$code_block
 */
function gen_lessthanexpected_arg_error_case($fn_name, $arg_det, $arg_c, $optional_args, $code_block) {

	$names = array_keys($arg_det);
	$types = array_values($arg_det);

	// check for no. of  mandatory arguments
	// if there are no mandatory arguments - return 
	// the code_block unchanged
	$mandatory_args = $arg_c - $optional_args;
	if($mandatory_args < 1) {
		return ($code_block);
	}

	//Discard optional arguments and last mandatory arg
	for ($i = 0; $i < $optional_args; $i++) {
		$discard_n = array_pop($names);
		$discard_v = array_pop($types);
	}
	$discard_n = array_pop($names);
	$discard_v = array_pop($types);

	array_push($code_block, "");
	array_push($code_block, "// Testing $fn_name with one less than the expected number of arguments");
        array_push($code_block, "echo \"\\n-- Testing $fn_name() function with less than expected no. of arguments --\\n\";");

	for($i = 0; $i < count($names); $i ++) {
		$i_stmt = get_variable_init_statement($types[$i], $names[$i]); 
		array_push($code_block, $i_stmt);
	}

	$all_args = "";
	if ($mandatory_args > 1) {
 		$var_name = array();
        	foreach ($names as $nm) {
               		array_push($var_name, "$".$nm);
        	}
        	$all_args = implode(", ", $var_name);
	}

        array_push($code_block, "var_dump( $fn_name($all_args) );");

        return $code_block;
}
/* 
 * Generates code for initalizing a given variable with value of same type
 * Arguments:
 * 	$var_type => data type of variable
 * 	$var_name => name of the variable
 * Returns:
 *	$code_block
 */
function get_variable_init_statement( $var_type, $var_name ) {
	$code = "";
	if ($var_type == "int") {
		$code = "\$$var_name = 10;";
	} else if($var_type == "float") {
		$code = "\$$var_name = 10.5;";
	} else if($var_type == "array") {
		$code = "\$$var_name = array(1, 2);";
	} else if($var_type == "string") {
		$code = "\$$var_name = 'string_val';";
	} else if($var_type == "object") {
		$code = "\$$var_name = new stdclass();";
	} else if($var_type == 'bool' || $var_type == 'boolean') {
		$code = "\$$var_name = true;";
	} else if($var_type == 'mixed') {
	// take a guess at int
		$code = "\$$var_name = 1;";
	} else {
	$code = "\n//WARNING: Unable to initialise $var_name of type $var_type\n";
}
  return $code;
}
/* 
 * Generate code for function with one argument
 * Arguments:
 *  	$fn_name => name of the function
 *  	$arg_name => name of the argument
 *  	$arg_type => data type of the argument
 *  	$code_block => an array of code that will be appended to
 * Returns:
 *  	$code_block
 */
function gen_one_arg_code($fn_name, $arg_name, $arg_type, $code_block) {
	//Initialse the argument
	$arg_one_init = get_variable_init_statement($arg_type, $arg_name);

        //push code onto the array $code_block
        array_push ($code_block, "// One argument");
        array_push ($code_block, "echo \"\\n-- Testing $fn_name() function with one argument --\\n\";");
        array_push ($code_block, "$arg_one_init;");
        array_push ($code_block, "var_dump( $fn_name(\$$arg_name) );");
  return $code_block;
}
/* 
 * Function to get the name of the source file in which the function is implemented and the proto comment
 * Arguments: 
 * 	$all_c => list of all c files in the PHP source
 * 	$fname => function name
 * 	$source => directory path of PHP source code
 * Returns:
 * 	$test_info = > array( source file, return type, parameters, falias, found, alias, error)
 */
function get_loc_proto($all_c, $fname, $source) {
//get location 
    $test_info['name'] = $fname;
    $test_info['source_file'] = NULL;
    $test_info['return_type'] = NULL;
    $test_info['params'] = NULL;
    $test_info['falias'] = false;
    $test_info['found'] = false;
    $test_info['alias'] = NULL;
    $test_info['error'] = NULL;
  
    $escaped_source = preg_replace("/\\\/", "\\\\\\", $source);
    $escaped_source = preg_replace("/\//", "\\\/", $escaped_source);
    
    
	for ($i=0; $i<count($all_c); $i++)
	{ 
		$strings=file_get_contents(chop($all_c[$i]));
		if (preg_match ("/FUNCTION\($fname\)/",$strings))
		{
			//strip build specific part of the implementation file name
			preg_match("/$escaped_source\/(.*)$/", $all_c[$i], $tmp);
			$test_info['source_file'] = $tmp[1];
			//get prototype information
			if (preg_match("/\/\*\s+{{{\s*proto\s+(\w*)\s*$fname\(\s*(.*)\s*\)(\n|)\s*(.*)\*\//", $strings, $matches)) {
				$test_info['return_type'] = $matches[1];
				$test_info['params'] = $matches[2];
				$test_info['desc'] = $matches[4];
			}
			else {
				$test_info['error'] = "Failed to  parse prototype for $fname in $all_c[$i] in function get_loc_proto()";
			}
			$test_info['found'] = true;
			if (preg_match ("/FALIAS\((\w+).*$fname,.*\)/",$strings, $alias_name)) {
			// This is not the main implementation, set falias to true
			$test_info['falias'] = true;
				if ( $test_info['alias'] != NULL) {
                               		$test_info['alias']  = $test_info['alias']." ".$alias_name[1];
                        	} else {
                                	$test_info['alias'] = $alias_name[1];
                        	}
			}
			break;
		}
		elseif (preg_match ("/FALIAS\($fname,\s*(\w+).*\)/",$strings, $alias_name)){
			// This is the main implementation but it has other functions aliased to it
			// Leave falias as false, but collect a list of the alias names
			if ( $test_info['alias'] != NULL) {
				$test_info['alias']  = $test_info['alias']." ".$alias_name[1];
			} else {
				$test_info['alias'] = $alias_name[1];
			}
			//strip build specific part of the implementation file name
			preg_match("/$escaped_source\/(.*)$/", $all_c[$i], $tmp);
			$test_info['source_file']= $tmp[1];
			$test_info['found'] = true;
		}
		//Some functions are in their own files and not declared using FUNTION/FALIAS. 
		//If we haven't found either FUNCTION or FAILIAS try just looking for the prototype 
		elseif (preg_match ("/\/\*\s+{{{\s*proto\s+(\w*)\s*$fname\(\s*(.*)\s*\)(\n|)\s*(.*)\*\//", $strings, $matches)) {
                        $test_info['return_type'] = $matches[1];
                        $test_info['params'] = $matches[2];
                        $test_info['desc'] = $matches[4];
                        $test_info['found'] = true;
			preg_match("/$escaped_source\/(.*)$/", $all_c[$i], $tmp);
                        $test_info['source_file']= $tmp[1];
                        break;
		}
	}
	return $test_info;
}
/*  
 * Generates code for basic functionality test. The generated code
 * will test the function with it's mandatory arguments and with all optional arguments.
 * Arguments:
 *	$fn_name => name of the function
 *	$arg_det => details of the each argument, stored in an array in the form of 'nameofargument' => 'typeofargument'
 *	$arg_c =>  total count of arguments that $fn_name takes
 *	$optional_args. $optional_args => total count of optional arguments that $fn_name takes
 *	$code_block - an array of code that will be appended to
 * Returns:
 * 	$code_block with appends
 */
function gen_basic_test_code($fn_name, $arg_det, $arg_c, $optional_args, $code_block) {
	if($arg_c == 0) {
		//Just generate Zero arg test case and return
		$code_block = gen_zero_arg_error_case($fn_name, $code_block);
		return $code_block;
	}
	$names = array_keys($arg_det);
	$types = array_values($arg_det);
  
	// prepare code to initialize all reqd. arguments
	array_push ($code_block, "");
	array_push ($code_block, "// Initialise all required variables");
	for($i = 0; $i < $arg_c; $i ++) {
		$i_stmt = get_variable_init_statement($types[$i], $names[$i]);
		array_push($code_block, $i_stmt);
	}


	// prepare the function calls

	// all arguments including the optional ones 
	$var_name = array();
        foreach ($names as $nm) {
                array_push($var_name, "$".$nm);
        }
        $all_args = implode(", ", $var_name);

	array_push ($code_block, "");
	array_push ($code_block, "// Calling $fn_name() with all possible arguments");
        array_push ($code_block, "var_dump( $fn_name($all_args) );");

	//now remove  the optional arguments and call the function with mandatory arguments only
	if ($optional_args != 0) {
		for ($i=0; $i < $optional_args; $i++) {
			$discard_n = array_pop($var_name);
		}
		$args = implode(", ", $var_name);
		array_push ($code_block, "");
		array_push ($code_block, "// Calling $fn_name() with mandatory arguments");
       		array_push ($code_block, "var_dump( $fn_name($args) );");
	}	
	return $code_block;
}

/*
 * Function to parse command line arguments
 * Returns:
 * 	$opt_array => array of options
 */
function initialise_opt() {
	$opt=array();
	$opt['source_loc'] = NULL;
	$opt['name'] = NULL;
	$opt['error_gen'] = false;
	$opt['basic_gen'] = false;
	$opt['variation_gen'] = false;
	$opt['include_block'] = NULL;
	return $opt;
}
function parse_args ($arglist, $opt) 
{
        for($j = 1; $j<count($arglist); $j++) {
                switch ($arglist[$j])
                {
                        case    '-s':
                                $j++;
                                $opt['source_loc'] = $arglist[$j];
                                break;

                        case    '-f':
                                $j++;
                                $opt['name'] = $arglist[$j];
                                break;

                        case    '-e':
                                $opt['error_gen'] = true;
                                break;

                        case    '-b':
                                $opt['basic_gen'] = true;
                                break;

                        case    '-v':
                                $opt['variation_gen'] = true;
                                break;
                                
                        case    '-i':
								$j++;
                                $opt['include_block'] = $arglist[$j];
                                break;
                                
                        case	'-h':
								print_opts();
								break;

                        default:
                                echo "Command line option $arg[$j] not recognised\n";
                                print_opts();
                }

        }
        return $opt;
}
/*
 * Function to check that source directory given for PHP source actually conatins PHP source
 */
function check_source($src) 
{
        $ext_loc = $src."/ext";
        if(!is_dir($ext_loc)) {
                echo "A PHP source location is required, $src does not appear to be a valid source location\n";
                print_opts();
        }
}
/*
 * Function to check that a file name exists
 */
function check_file($f) 
{
        if(!is_file($f)) {
                echo "$f is not a valid file name \n";
                print_opts();
        }
}
/*
 * Function to check thet either a file name, a list of files or a function area has been supplied
 */
function check_fname ($f)
{
        if($f == NULL ) { 
                echo "Require a function name \n";
                print_opts();
        }
}
/*
 * Function to check that the user has requested a type of test case
 */
function check_testcase($e, $v, $b) {
        if(!$v && !$e && !$b) { 
                echo "Need to specify which type of test to generate\n";
                print_opts();
        }
}
/*
 * Function to print out help text if any of the input data is incorrect
 */
function print_opts() {
		echo "\nUsage:\n";
		echo "      php generate_phpt.php -s <location_of_source_code> -f <function_name> -b|e|v [-i file]\n\n";
        echo "-s location_of_source_code ....... Top level directory of PHP source\n";
        echo "-f function_name ................. Name of PHP function, eg cos\n";
        echo "-b ............................... Generate basic tests\n";
        echo "-e ............................... Generate error tests\n";
        echo "-v ............................... Generate variation tests\n";
        echo "-i file_containing_include_block.. Block of PHP code to be included in the test case\n";
        echo "-h ............................... Print this message\n";
        exit;
}
/* 
 * Generates a general testcase template and create the testcase file,
 * No code is added other than header and trailer
 * Arguments:
 *	$fn_det => Array with function details
 * 	$sections => List of test sections eg '--TEST--', '--FILE--'..
 * 	$type =>  basic/error/variation 
 * 	$php_file => name of file to import PHP code from
 * Returns:
 * 	$test_case => an array containing the complete test case
 */
function gen_template($fn_det, $sections, $type, $php_file=NULL) {
        $name = $fn_det['name'];
        $proto = $fn_det['proto'];
        $desc = $fn_det['desc'];
        $source_file = $fn_det['source_file'];
        $alias = $fn_det['alias'];

	$test_case = array();

	// get the test header and write into the file
	$test_case = gen_test_header($name, $proto, $desc, $source_file, $type, "",$alias, $test_case);

	// write the message to indicate the start of addition of code in the template file
	if ($php_file == NULl) {
		$msg = "\n // add test code here \n";
		array_push($test_case['--FILE--'], $msg);
	}else{
		$test_case['--FILE--'] = read_include_file($php_file, $test_case['--FILE--']);
	}

	// end the script
	$test_case = gen_test_trailer($test_case);
	write_file($test_case, $name, $type, $sections);
	return ($test_case);
}
/* 
 * Generate code for testing zero argument case for error testcase
 * Arguments:
 * 	$fn_name => name of the function
 * 	$code_block => array of code which will be appended to
 * Returns:
 * 	$code_block
 */ 
function gen_zero_arg_error_case($fn_name, $code_block) {
	//push code onto the array $code_block
	array_push ($code_block, "// Zero arguments");
	array_push ($code_block, "echo \"\\n-- Testing $fn_name() function with Zero arguments --\\n\";");
	array_push ($code_block, "var_dump( $fn_name() );");
  return $code_block;
}
?>
