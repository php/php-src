<?php
class gtOptionalSections {

  private $optSections = array(
    'skipif' => false,
    'ini'    => false,
    'clean'  => false,
    'done'   => false,
  );

  private $skipifKey = '';
  private $skipifExt = '';


  public function setOptions($commandLineOptions) {
    if($commandLineOptions->hasOption('s')) {
      $options = explode(':', $commandLineOptions->getOption('s'));

      foreach($options as $option) {

        if(array_key_exists($option, $this->optSections )) {
          $this->optSections[$option] = true;
        } else {
          throw new gtUnknownSectionException('Unrecognised optional section');
        }
      }

      if($commandLineOptions->hasOption('k')) {
        $this->skipifKey = $commandLineOptions->getOption('k');
      }
      
      if($commandLineOptions->hasOption('x')) {
        $this->skipifExt = $commandLineOptions->getOption('x');
      }
      
    }
  }



  public function getOptions() {
    return $this->optSections;
  }


  public function getSkipifKey() {
    return $this->skipifKey;
  }
  
  public function getSkipifExt() {
    return $this->skipifExt;
  }

  public function hasSkipif() {
    return $this->optSections['skipif'];
  }

  public function hasSkipifKey() {
    if($this->skipifKey != '') {
      return true;
    }
    return false;
  }
  
  public function hasSkipifExt() {
    if($this->skipifExt != '') {
      return true;
    }
    return false;
  }
  public function hasIni() {
    return $this->optSections['ini'];
  }

  public function hasClean() {
    return $this->optSections['clean'];
  }
  
  public function hasDone() {
    return $this->optSections['done'];
  }


}
?>