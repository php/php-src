<?php

require_once "PEAR.php";

class PEAR_CommandUI_CLI extends PEAR
{
    var $output_mode = 'plain';
    var $output_mode_params = array();

    function PEAR_CommandUI_CLI()
    {
        parent::PEAR();
    }

    function _PEAR_CommandUI_CLI()
    {
        parent::_PEAR();
        if ($this->output_mode) {
            $this->endOutput();
        }
    }

    function displayLine($text)
    {
        print "$text\n";
    }

    function userDialog($prompt, $type = 'text', $default = '')
    {
        if ($type == 'password') {
            system('stty -echo');
        }
        print "$prompt ";
        if ($default) {
            print "[$default] ";
        }
        print ": ";
        $fp = fopen("php://stdin", "r");
        $line = fgets($fp, 2048);
        fclose($fp);
        if ($type == 'password') {
            system('stty echo');
            print "\n";
        }
        if ($default && trim($line) == "") {
            return $default;
        }
        return $line;
    }

    function userConfirm($prompt, $default = 'yes')
    {
        static $positives = array('y', 'yes', 'on', '1');
        static $negatives = array('n', 'no', 'off', '0');
        print "$prompt [$default] : ";
        $fp = fopen("php://stdin", "r");
        $line = fgets($fp, 2048);
        fclose($fp);
        $answer = strtolower(trim($line));
        if (empty($answer)) {
            $answer = $default;
        }
        if (in_array($answer, $positives)) {
            return true;
        }
        if (in_array($answer, $negatives)) {
            return false;
        }
        if (in_array($default, $positives)) {
            return true;
        }
        return false;
    }

    function setOutputMode($mode, $params = array())
    {
        $this->output_mode = $mode;
        $this->output_mode_params = $params;
    }

    function startOutput($mode)
    {
        if ($this->output_mode) {
            $this->endOutput();
        }
        switch ($mode) {
        }
    }

    function endOutput($mode = null)
    {
        if ($mode === null) {
            $mode = $this->output_mode;
        }
        $this->output_mode = '';
        switch ($mode) {
        }
    }
}

?>