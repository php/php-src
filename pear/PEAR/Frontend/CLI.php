<?php

require_once "PEAR.php";

class PEAR_Frontend_CLI extends PEAR
{
    var $omode = 'plain';
    var $params = array();

    function PEAR_Frontend_CLI()
    {
        parent::PEAR();
    }

    function _PEAR_Frontend_CLI()
    {
        parent::_PEAR();
        if ($this->omode) {
            $this->endOutput();
        }
    }

    function displayLine($text)
    {
        print "| $text\n";
    }

    function displayHeading($title)
    {
        print "| ".strtoupper($title)."\n";
        print "| ".str_repeat("=", strlen($title))."\n";
    }

    function userDialog($prompt, $type = 'text', $default = '')
    {
        if ($type == 'password') {
            system('stty -echo');
        }
        print "| $prompt ";
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
        print "| $prompt [$default] : ";
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

    function startTable($params = array())
    {
        $this->omode = 'table';
        $params['table_data'] = array();
        $params['widest'] = array();
        $params['ncols'] = 0;
        $this->params = $params;
    }

    function tableRow($columns, $rowparams = array(), $colparams = array())
    {
        for ($i = 0; $i < sizeof($columns); $i++) {
            if (!isset($this->params['widest'][$i]) ||
                strlen($columns[$i] > $this->params['widest'][$i]))
            {
                $this->params['widest'][$i] = strlen($columns[$i]);
            }
        }
        if (!isset($this->params['ncols']) ||
            sizeof($columns) > $this->params['ncols'])
        {
            $this->params['ncols'] = sizeof($columns);
        }
        $new_row = array(
            'data' => $columns,
            'rowparams' => $rowparams,
            'colparams' => $colparams,
            );
        $this->params['table_data'][] = $new_row;
    }

    function endTable()
    {
        $this->omode = '';
        extract($this->params);
        if (!empty($caption)) {
            $this->displayHeading($caption);
        }
        if (!isset($width)) {
            $width = $widest;
        } else {
            for ($i = 0; $i < $ncols; $i++) {
                if (!isset($width[$i])) {
                    $width[$i] = $widest[$i];
                }
            }
        }
        if (empty($params['border'])) {
            $cellstart = " ";
            $rowend = "\n";
            $padrowend = false;
        } else {
            $cellstart = "| ";
            $rowend = " |\n";
            $padrowend = true;
            $borderline = "+";
            foreach ($width as $w) {
                $borderline .= str_repeat('-', $w + 2);
            }
        }
        for ($i = 0; $i < sizeof($table_data); $i++) {
            extract($table_data[$i]);
            for ($c = 0; $c < sizeof($data); $c++) {
                $w = $width[$c];
                $l = strlen($data[$c]);
                if ($l > $w) {
                    print
                }
            }
        }
    }
}

?>