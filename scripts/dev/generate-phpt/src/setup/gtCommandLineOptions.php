<?php

/**
 * Parse command line options
 *
 */
class gtCommandLineOptions {

  protected $shortOptions = array(
    'b',
    'e',
    'v',
    'h',
  );

  protected $shortOptionsWithArgs = array(
    'c',
    'm',
    'f',
    'i',
    's',
    'x',
    'k',
  );
  
  protected $options;

  protected function isShortOption($arg)
  {
    return (substr($arg, 0, 1) == '-') && (substr($arg, 1, 1) != '-');
  }

  public function isValidOptionArg($array, $index) {
    if (!isset($array[$index]))
    {
      return false;
    }
    return substr($array[$index], 0, 1) != '-';
  }


  public function parse($argv)
  {
    if(count($argv) < 2) {
      throw new gtMissingOptionsException('Command line options are required');
    }
    
    for ($i=1; $i<count($argv); $i++) {

      if ($this->isShortOption($argv[$i])) {
        $option = substr($argv[$i], 1);
      } else {
        throw new gtUnknownOptionException('Unrecognised command line option ' . $argv[$i]);
      }

      if (!in_array($option, array_merge($this->shortOptions, $this->shortOptionsWithArgs)))
      {
        throw new gtUnknownOptionException('Unknown option ' . $argv[$i]);
      }

      if (in_array($option, $this->shortOptions)) {
        $this->options[$option] = true;
        continue;
      }

      if (!$this->isValidOptionArg($argv, $i + 1))
      {
        throw new gtMissingArgumentException('Missing argument for command line option ' . $argv[$i]);
      }

      $i++;
      $this->options[$option] = $argv[$i];
    }
  }
  
 /**
   *
   */
  public function getOption($option)
  {
    if (!isset($this->options[$option])) {
      return false;
    }
    return $this->options[$option];
  }


  /**
   * Check whether an option exists
   */
  public function hasOption($option)
  {
    return isset($this->options[$option]);
  }


}
?>