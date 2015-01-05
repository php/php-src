
<?php

/**
 * Class for extended variations. Needs 'data type' and argument to vary
 */

abstract class gtVariationTestCase extends gtTestCase {


  /**
   * Returns an instance of a test case for a method or a function
   *
   * @param string $type
   * @return test case object
   */
  public static function getInstance($optionalSections, $type = 'function') {
     
    if($type == 'function') {
      return new gtVariationTestCaseFunction($optionalSections);
    }
    if($type =='method') {
      return new gtVariationTestCaseMethod($optionalSections);
    }

  }

  public function argInitVariation() {
    $statements = $this->subject->getInitialisationStatements();
    for($i=0; $i<count($statements); $i++) {
      if($i != ( $this->argumentNumber -1) ) {
        $this->testCase[] = $statements[$i];
      }
    }
    $this->testCase = gtCodeSnippet::appendBlankLines(2, $this->testCase );
  }

  public function addVariationCode() {
    $this->testCase = gtCodeSnippet::append($this->variationData, $this->testCase);
    $this->testCase = gtCodeSnippet::appendBlankLines(2, $this->testCase );
  }

  public function constructSubjectCalls() {
    $this->argInitVariation();
    $this->addVariationCode();
    $this->subjectCalls();
  }

  public function addVariationEcho() {
    $this->testCase[] = "echo \"*** Test substituting argument ".$this->argumentNumber." with ".$this->variationData." values ***\\n\";";
    $this->testCase = gtCodeSnippet::appendBlankLines(1, $this->testCase );
  }

}
?>