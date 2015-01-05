<?php
/**
 * Main code for test case generation
 */

require_once dirname(__FILE__) . '/gtAutoload.php';

//Version check. Will not run on less than PHP53;

list($major, $minor, $bug) = explode(".", phpversion(), 3);
 if($major == 5) {
       if($minor < 3) { die("Sorry, you need PHP version 5.3 or greater to run this.\n"); }
   }
   if ($major < 5) { die ("Seriously, you need to upgrade you PHP level\n"); }


$options = new gtCommandLineOptions();
$optionalSections = new gtOptionalSections();

try{
  $options->parse($argv);
} catch (exception $e) {
  echo $e->getMessage()."\n";
  die();
}

if($options->hasOption('h')) {
  die(gtText::get('help'));
}

try {
  $preConditions = new gtPreConditionList();
  $preConditions->check($options);
} catch (exception $e) {
  echo $e->getMessage()."\n";
  die();
}

if($options->hasOption('s')) {
  $optionalSections->setOptions($options);
}
  


if($options->hasOption('c')) {
  $name = $options->getOption('c')."_".$options->getOption('m');
  $method = new gtMethod($options->getOption('c'), $options->getOption('m'));
  
  $method->setArgumentNames();
  $method->setArgumentLists();
  $method->setInitialisationStatements();
  
  $method->setConstructorArgumentNames();
  $method->setConstructorInitStatements();
  $method->setConstructorArgumentList();
}

if($options->hasOption('f')) {
  $name = $options->getOption('f');
  $function = new gtFunction($name);
  $function->setArgumentNames();
  $function->setArgumentLists();
  $function->setInitialisationStatements();
}


if($options->hasOption('b')) {
  if($options->hasOption('c')) {
    $testCase = gtBasicTestCase::getInstance($optionalSections, 'method');
    $testCase->setMethod($method);
  } else {
    $testCase = gtBasicTestCase::getInstance($optionalSections);
    $testCase->setFunction($function);
  }

  $testCase->constructTestCase();
  gtTestCaseWriter::write($name, $testCase->toString(), 'b');
}

if($options->hasOption('e')) {
  if($options->hasOption('c')) {
    $testCase = gtErrorTestCase::getInstance($optionalSections, 'method');
    $testCase->setMethod($method);
  } else {
    $testCase = gtErrorTestCase::getInstance($optionalSections);
    $testCase->setFunction($function);
  }

  $testCase->constructTestCase();
  gtTestCaseWriter::write($name, $testCase->toString(), 'e');
}



if($options->hasOption('v')) {
  if($options->hasOption('c')) {
    $testCaseContainer = gtVariationContainer::getInstance($optionalSections, 'method');
    $testCaseContainer->setMethod($method);
  } else {
    $testCaseContainer = gtVariationContainer::getInstance ($optionalSections);
    $testCaseContainer->setFunction($function);
  }

  $testCaseContainer->constructAll();

  $tests = $testCaseContainer->getVariationTests();

  $count = 1;
  foreach($tests as $test) {
    gtTestCaseWriter::write($name, $test, 'v', $count);
    $count++;
  }

}
?>
